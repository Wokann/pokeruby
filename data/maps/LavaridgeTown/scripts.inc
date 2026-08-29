LavaridgeTown_MapScripts:: @ 814E4A3
	map_script MAP_SCRIPT_ON_TRANSITION, LavaridgeTown_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LavaridgeTown_OnFrame
	.byte 0

LavaridgeTown_OnTransition:: @ 814E4AE
	setflag FLAG_VISITED_LAVARIDGE_TOWN
	call_if_set FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, LavaridgeTown_EventScript_ShowMtChimneyTrainers
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupRivalOnBikeGfxId
	call_if_eq VAR_LAVARIDGE_TOWN_STATE, 2, LavaridgeTown_EventScript_SetRivalPos
	end

LavaridgeTown_EventScript_SetRivalPos:: @ 814E4D0
	setobjectxyperm LOCALID_LAVARIDGE_RIVAL, 11, 9
	setobjectxyperm LOCALID_LAVARIDGE_RIVAL_ON_BIKE, 11, 9
	return

LavaridgeTown_EventScript_ShowMtChimneyTrainers:: @ 814E4DF
	clearflag FLAG_HIDE_MT_CHIMNEY_PEOPLE
	return

LavaridgeTown_OnFrame:: @ 814E4E3
	map_script_2 VAR_LAVARIDGE_TOWN_STATE, 1, LavaridgeTown_EventScript_RivalGiveGoGoggles
	.2byte 0

LavaridgeTown_EventScript_RivalGiveGoGoggles:: @ 814E4ED
	lockall
	setvar VAR_0x8008, TRUE
	opendoor 12, 15
	waitdooranim
	addobject LOCALID_LAVARIDGE_RIVAL
	applymovement LOCALID_LAVARIDGE_RIVAL, LavaridgeTown_Movement_RivalExitHerbShop
	waitmovement 0
	closedoor 12, 15
	waitdooranim
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_Delay48
	waitmovement 0
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LavaridgeTown_EventScript_PlayMayMusic
	call_if_eq VAR_RESULT, FEMALE, LavaridgeTown_EventScript_PlayBrendanMusic
	call_if_eq VAR_0x8008, TRUE, LavaridgeTown_EventScript_RivalApproachPlayerFromHerbShop
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LavaridgeTown_EventScript_MayGiveGoGoggles
	goto_if_eq VAR_RESULT, FEMALE, LavaridgeTown_EventScript_BrendanGiveGoGoggles
	end

LavaridgeTown_EventScript_PlayMayMusic:: @ 814E56B
	playbgm MUS_ENCOUNTER_MAY, TRUE
	return

LavaridgeTown_EventScript_PlayBrendanMusic:: @ 814E570
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	return

LavaridgeTown_EventScript_RivalGiveGoGogglesAtHerbShop:: @ 814E575
	lock
	faceplayer
	setvar VAR_0x8008, FALSE
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LavaridgeTown_EventScript_PlayMayMusic
	call_if_eq VAR_RESULT, FEMALE, LavaridgeTown_EventScript_PlayBrendanMusic
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LavaridgeTown_EventScript_MayGiveGoGoggles
	goto_if_eq VAR_RESULT, FEMALE, LavaridgeTown_EventScript_BrendanGiveGoGoggles
	end

LavaridgeTown_EventScript_MayGiveGoGoggles:: @ 814E5AB
	msgbox LavaridgeTown_Text_MayNiceBadgesTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_GO_GOGGLES
	setflag FLAG_RECEIVED_GO_GOGGLES
	msgbox LavaridgeTown_Text_MayExplainGoGogglesChallengeDad, MSGBOX_DEFAULT
	goto LavaridgeTown_EventScript_RivalExit
	end

LavaridgeTown_EventScript_BrendanGiveGoGoggles:: @ 814E5D0
	msgbox LavaridgeTown_Text_BrendanNiceBadgesTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_GO_GOGGLES
	setflag FLAG_RECEIVED_GO_GOGGLES
	msgbox LavaridgeTown_Text_BrendanExplainGoGogglesChallengeDad, MSGBOX_DEFAULT
	goto LavaridgeTown_EventScript_RivalExit
	end

LavaridgeTown_EventScript_RivalExit:: @ 814E5F5
	closemessage
	call_if_eq VAR_0x8008, TRUE, LavaridgeTown_EventScript_SetRivalOnBikePosition
	call_if_eq VAR_0x8008, FALSE, LavaridgeTown_EventScript_SetRivalOnBikeFacing
	removeobject LOCALID_LAVARIDGE_RIVAL
	addobject LOCALID_LAVARIDGE_RIVAL_ON_BIKE
	delay 30
	call_if_eq VAR_0x8008, TRUE, LavaridgeTown_EventScript_RivalExit2
	call_if_eq VAR_0x8008, FALSE, LavaridgeTown_EventScript_RivalExitBasedOnPlayerFacing
	removeobject LOCALID_LAVARIDGE_RIVAL_ON_BIKE
	setvar VAR_LAVARIDGE_TOWN_STATE, 2
	savebgm 0
	fadedefaultbgm
	goto_if_eq VAR_0x8008, FALSE, LavaridgeTown_EventScript_ReleaseAfterRivalExit
	releaseall
	end

LavaridgeTown_EventScript_ReleaseAfterRivalExit:: @ 814E644
	release
	end

LavaridgeTown_EventScript_SetRivalOnBikePosition:: @ 814E646
	setobjectxyperm LOCALID_LAVARIDGE_RIVAL_ON_BIKE, 6, 16
	return

LavaridgeTown_EventScript_SetRivalOnBikeFacing:: @ 814E64E
	call_if_eq VAR_FACING, DIR_NORTH, LavaridgeTown_EventScript_SetRivalOnBikeFaceDown
	call_if_eq VAR_FACING, DIR_SOUTH, LavaridgeTown_EventScript_SetRivalOnBikeFaceUp
	call_if_eq VAR_FACING, DIR_WEST, LavaridgeTown_EventScript_SetRivalOnBikeFaceRight
	call_if_eq VAR_FACING, DIR_EAST, LavaridgeTown_EventScript_SetRivalOnBikeFaceLeft
	return

LavaridgeTown_EventScript_SetRivalOnBikeFaceDown:: @ 814E67B
	setobjectmovementtype LOCALID_LAVARIDGE_RIVAL_ON_BIKE, MOVEMENT_TYPE_FACE_DOWN
	return

LavaridgeTown_EventScript_SetRivalOnBikeFaceUp:: @ 814E680
	setobjectmovementtype LOCALID_LAVARIDGE_RIVAL_ON_BIKE, MOVEMENT_TYPE_FACE_UP
	return

LavaridgeTown_EventScript_SetRivalOnBikeFaceRight:: @ 814E685
	setobjectmovementtype LOCALID_LAVARIDGE_RIVAL_ON_BIKE, MOVEMENT_TYPE_FACE_RIGHT
	return

LavaridgeTown_EventScript_SetRivalOnBikeFaceLeft:: @ 814E68A
	setobjectmovementtype LOCALID_LAVARIDGE_RIVAL_ON_BIKE, MOVEMENT_TYPE_FACE_LEFT
	return

LavaridgeTown_EventScript_RivalApproachPlayerFromHerbShop:: @ 814E68F
	applymovement LOCALID_LAVARIDGE_RIVAL, LavaridgeTown_Movement_RivalApproachPlayer2
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalExit2:: @ 814E69A
	applymovement LOCALID_LAVARIDGE_RIVAL_ON_BIKE, LavaridgeTown_Movement_RivalExit2
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalExitBasedOnPlayerFacing:: @ 814E6A5
	call_if_eq VAR_FACING, DIR_NORTH, LavaridgeTown_EventScript_RivalExitEast
	call_if_eq VAR_FACING, DIR_SOUTH, LavaridgeTown_EventScript_RivalExitEast
	call_if_eq VAR_FACING, DIR_WEST, LavaridgeTown_EventScript_RivalExit1
	call_if_eq VAR_FACING, DIR_EAST, LavaridgeTown_EventScript_RivalExitEast
	return

LavaridgeTown_EventScript_RivalExitEast:: @ 814E6D2
	applymovement LOCALID_LAVARIDGE_RIVAL_ON_BIKE, LavaridgeTown_Movement_RivalExitEast
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalExit1:: @ 814E6DD
	applymovement LOCALID_LAVARIDGE_RIVAL_ON_BIKE, LavaridgeTown_Movement_RivalExit1
	waitmovement 0
	return

LavaridgeTown_Movement_RivalApproachPlayer2:: @ 814E6E8
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

LavaridgeTown_Movement_RivalExit2:: @ 814E6EF
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

LavaridgeTown_Movement_RivalExitEast:: @ 814E6FA
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

LavaridgeTown_Movement_RivalExit1:: @ 814E703
	walk_fast_down
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

LavaridgeTown_Movement_RivalExitHerbShop:: @ 814E70E
	walk_down
	step_end

LavaridgeTown_EventScript_HotSpringsTrigger:: @ 814E710
	specialvar VAR_RESULT, GetPlayerFacingDirection
	goto_if_eq VAR_RESULT, DIR_SOUTH, LavaridgeTown_EventScript_EnteredHotSprings
	end

LavaridgeTown_EventScript_EnteredHotSprings:: @ 814E721
	incrementgamestat GAME_STAT_ENTERED_HOT_SPRINGS
	end

LavaridgeTown_EventScript_ExpertM:: @ 814E724
	msgbox LavaridgeTown_Text_HotSpringsNeverRunDry, MSGBOX_NPC
	end

LavaridgeTown_EventScript_OldMan:: @ 814E72D
	msgbox LavaridgeTown_Text_PokemonNippedBackside, MSGBOX_SIGN
	end

LavaridgeTown_EventScript_Twin:: @ 814E736
	msgbox LavaridgeTown_Text_BatheInHotSpringsEveryDay, MSGBOX_NPC
	end

LavaridgeTown_EventScript_HotSpringsOldWoman1:: @ 814E73F
	msgbox LavaridgeTown_Text_IfPokemonInHotSprings, MSGBOX_NPC
	end

LavaridgeTown_EventScript_HotSpringsOldWoman2:: @ 814E748
	msgbox LavaridgeTown_Text_HotSpringsClaims, MSGBOX_NPC
	end

LavaridgeTown_EventScript_ExpertF:: @ 814E751
	msgbox LavaridgeTown_Text_OhYouLikeHotSprings, MSGBOX_NPC
	end

LavaridgeTown_EventScript_EggWoman:: @ 814E75A
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_LAVARIDGE_EGG, LavaridgeTown_EventScript_ReceivedEgg
	msgbox LavaridgeTown_Text_HaveEggWillYouTakeIt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, LavaridgeTown_EventScript_DeclineEgg
	getpartysize
	goto_if_eq VAR_RESULT, PARTY_SIZE, LavaridgeTown_EventScript_NoRoomForEgg
	msgbox LavaridgeTown_Text_HopeYoullWalkPlentyWithEgg, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_LAVARIDGE_EGG
	playfanfare MUS_OBTAIN_ITEM
	message LavaridgeTown_Text_ReceivedTheEgg
	waitfanfare
	waitmessage
	giveegg SPECIES_WYNAUT
	release
	end

LavaridgeTown_EventScript_ReceivedEgg:: @ 814E79E
	msgbox LavaridgeTown_Text_EverySoOftenEggFoundAtDayCare, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_EventScript_NoRoomForEgg:: @ 814E7A8
	msgbox LavaridgeTown_Text_NoRoomForThisEgg, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_EventScript_DeclineEgg:: @ 814E7B2
	msgbox LavaridgeTown_Text_AsYouWishThen, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_EventScript_TownSign:: @ 814E7BC
	msgbox LavaridgeTown_Text_TownSign, MSGBOX_SIGN
	end

LavaridgeTown_EventScript_GymSign:: @ 814E7C5
	msgbox LavaridgeTown_Text_GymSign, MSGBOX_SIGN
	end

LavaridgeTown_EventScript_UnusedHerbShopSign:: @ 814E7CE
	msgbox LavaridgeTown_Text_HerbShopSign, MSGBOX_SIGN
	end

LavaridgeTown_EventScript_HerbShopSign:: @ 814E7D7
	msgbox LavaridgeTown_Text_HerbShopSign, MSGBOX_SIGN
	end
