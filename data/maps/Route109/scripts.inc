Route109_MapScripts:: @ 814F4D2
	.byte 0

Route109_EventScript_StartDepartForDewford:: @ 814F4D3
	call EventScript_BackupMrBrineyLocation
	setobjectpriority LOCALID_ROUTE109_BRINEY, MAP_ROUTE109, 0
	setobjectpriority LOCALID_PLAYER, MAP_ROUTE109, 0
	applymovement LOCALID_ROUTE109_BRINEY, Route109_Movement_BrineyEnterBoat
	waitmovement 0
	removeobject LOCALID_ROUTE109_BRINEY
	switch VAR_FACING
	case 1, Route109_EventScript_EnterBoatSouth
	case 3, Route109_EventScript_EnterBoatWest
	case 4, Route109_EventScript_EnterBoatEast
	end

Route109_EventScript_EnterBoatSouth:: @ 814F518
	applymovement LOCALID_PLAYER, Route109_Movement_PlayerEnterBoatSouth
	waitmovement 0
	goto Route109_EventScript_DoSailToDewford
	end

Route109_EventScript_EnterBoatEast:: @ 814F528
	applymovement LOCALID_PLAYER, Route109_Movement_PlayerEnterBoatEast
	waitmovement 0
	goto Route109_EventScript_DoSailToDewford
	end

Route109_EventScript_EnterBoatWest:: @ 814F538
	applymovement LOCALID_PLAYER, Route109_Movement_PlayerEnterBoatWest
	waitmovement 0
	goto Route109_EventScript_DoSailToDewford
	end

Route109_EventScript_DoSailToDewford:: @ 814F548
	hideobjectat LOCALID_PLAYER, MAP_ROUTE109
	call Common_EventScript_PlayBrineysBoatMusic
	applymovement LOCALID_ROUTE109_BOAT, Route109_Movement_SailToDewford
	applymovement LOCALID_PLAYER, Route109_Movement_SailToDewford
	waitmovement 0
	call Common_EventScript_StopBrineysBoatMusic
	showobjectat LOCALID_PLAYER, MAP_DEWFORD_TOWN
	applymovement LOCALID_PLAYER, Route109_Movement_PlayerExitBoat
	waitmovement 0
	addobject LOCALID_DEWFORD_BOAT
	clearflag FLAG_HIDE_MR_BRINEY_BOAT_DEWFORD_TOWN
	setobjectxyperm LOCALID_DEWFORD_BRINEY, 12, 8
	addobject LOCALID_DEWFORD_BRINEY
	setobjectpriority LOCALID_DEWFORD_BRINEY, MAP_DEWFORD_TOWN, 0
	applymovement LOCALID_DEWFORD_BRINEY, Route109_Movement_BrineyExitBoat
	waitmovement 0
	clearflag FLAG_HIDE_MR_BRINEY_DEWFORD_TOWN
	setflag FLAG_HIDE_MR_BRINEY_BOAT_ROUTE109
	hideobjectat LOCALID_ROUTE109_BOAT, MAP_ROUTE109
	msgbox Route109_Text_BrineyLandedInDewford, MSGBOX_DEFAULT
	closemessage
	copyvar VAR_BRINEY_LOCATION, VAR_0x8008
	resetobjectpriority LOCALID_PLAYER, MAP_ROUTE109
	resetobjectpriority LOCALID_DEWFORD_BRINEY, MAP_DEWFORD_TOWN
	moveobjectoffscreen LOCALID_DEWFORD_BRINEY
	release
	end

Route109_Movement_SailToDewford:: @ 814F5BF
	walk_in_place_fastest_down
	walk_down
	walk_down
	walk_fast_down
	walk_fast_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fastest_down
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fastest_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_left
	walk_left
	walk_in_place_fastest_down
	step_end

Route109_Movement_PlayerEnterBoatSouth:: @ 814F66D
	walk_down
	walk_down
	step_end

Route109_Movement_PlayerExitBoat:: @ 814F670
	walk_down
	walk_down
	walk_left
	walk_in_place_fastest_right
	step_end

Route109_Movement_PlayerEnterBoatEast:: @ 814F675
	walk_right
	walk_down
	step_end

Route109_Movement_PlayerEnterBoatWest:: @ 814F678
	walk_left
	walk_down
	step_end

Route109_Movement_BrineyEnterBoat:: @ 814F67B
	walk_down
	step_end

Route109_Movement_BrineyExitBoat:: @ 814F67D
	walk_down
	walk_in_place_fastest_left
	step_end

Route109_EventScript_MrBriney:: @ 814F680
	lock
	faceplayer
	goto_if_unset FLAG_DELIVERED_DEVON_GOODS, Route109_EventScript_HaveNotDeliveredDevonGood
	goto Route109_EventScript_DeliveredDevonGoods
	end

Route109_EventScript_HaveNotDeliveredDevonGood:: @ 814F691
	message Route109_Text_BrineySailToDewfordQuestion
	msgbox Route109_Text_BrineySailToDewfordQuestion, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route109_EventScript_StayHere
	goto Route109_EventScript_SailToDewford
	end

Route109_EventScript_DeliveredDevonGoods:: @ 814F6AF
	message Route109_Text_BrineyWhereAreWeBound
	waitmessage
	multichoicedefault 21, 8, 14, 1, 0
	switch VAR_RESULT
	case 0, Route109_EventScript_SailToDewford
	case 1, Route109_EventScript_ChoseNotToSail
	case 127, Route109_EventScript_ChoseNotToSail
	end

Route109_EventScript_SailToDewford:: @ 814F6E2
	msgbox Route109_Text_BrineyDewfordItIs, MSGBOX_DEFAULT
	closemessage
	goto Route109_EventScript_StartDepartForDewford
	end

Route109_EventScript_StayHere:: @ 814F6F1
	msgbox Route109_Text_BrineyDeliverDevonGoods, MSGBOX_DEFAULT
	release
	end

Route109_EventScript_ChoseNotToSail:: @ 814F6FB
	msgbox Route109_Text_BrineyTellMeWhenYouNeedToSail, MSGBOX_DEFAULT
	release
	end

Route109_EventScript_SeashoreHouseGirl:: @ 814F705
	msgbox Route109_Text_ChillAtMyPapasSpot, MSGBOX_NPC
	end

Route109_EventScript_SandCastleBoy:: @ 814F70E
	lock
	faceplayer
	msgbox Route109_Text_SandCastleTakingLongTime, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route109_EventScript_SoftSandGirl:: @ 814F725
	lock
	faceplayer
	special GetPlayerBigGuyGirlString
	goto_if_set FLAG_RECEIVED_SOFT_SAND, Route109_EventScript_AlreadyReceivedSoftSand
	msgbox Route109_Text_YouCanHaveThis, MSGBOX_DEFAULT
	giveitem ITEM_SOFT_SAND
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	setflag FLAG_RECEIVED_SOFT_SAND
	release
	end

Route109_EventScript_AlreadyReceivedSoftSand:: @ 814F762
	msgbox Route109_Text_WereGoingToMakeBigCastle, MSGBOX_DEFAULT
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route109_EventScript_Woman:: @ 814F776
	msgbox Route109_Text_LittleKidsDartAround, MSGBOX_NPC
	end

Route109_EventScript_OldMan:: @ 814F77F
	msgbox Route109_Text_ZigzagoonPicksUpLitter, MSGBOX_NPC
	end

Route109_EventScript_Zigzagoon:: @ 814F788
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, 0
	msgbox Route109_Text_ZigzagoonCry, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route109_EventScript_SeashoreHouseSign:: @ 814F79B
	msgbox Route109_Text_SeashoreHouseSign, MSGBOX_SIGN
	end

Route109_EventScript_TrainerTipsSign:: @ 814F7A4
	msgbox Route109_Text_TrainerTipsSign, MSGBOX_SIGN
	end

Route109_EventScript_David:: @ 814F7AD
	trainerbattle_single TRAINER_DAVID, Route109_Text_DavidIntro, Route109_Text_DavidDefeated
	msgbox Route109_Text_DavidPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Alice:: @ 814F7C4
	trainerbattle_single TRAINER_ALICE, Route109_Text_AliceIntro, Route109_Text_AliceDefeated
	msgbox Route109_Text_AlicePostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Huey:: @ 814F7DB
	trainerbattle_single TRAINER_HUEY, Route109_Text_HueyIntro, Route109_Text_HueyDefeated
	msgbox Route109_Text_HueyPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Edmond:: @ 814F7F2
	trainerbattle_single TRAINER_EDMOND, Route109_Text_EdmondIntro, Route109_Text_EdmondDefeated
	msgbox Route109_Text_EdmondPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Ricky:: @ 814F809
	trainerbattle_single TRAINER_RICKY_1, Route109_Text_RickyIntro, Route109_Text_RickyDefeated
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route109_EventScript_RickyRematch
	msgbox Route109_Text_RickyPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_RickyRematch:: @ 814F830
	trainerbattle_rematch TRAINER_RICKY_1, Route109_Text_RickyRematchIntro, Route109_Text_RickyRematchDefeated
	msgbox Route109_Text_RickyRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Lola:: @ 814F847
	trainerbattle_single TRAINER_LOLA_1, Route109_Text_LolaIntro, Route109_Text_LolaDefeated
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route109_EventScript_LolaRematch
	msgbox Route109_Text_LolaPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_LolaRematch:: @ 814F86E
	trainerbattle_rematch TRAINER_LOLA_1, Route109_Text_LolaRematchIntro, Route109_Text_LolaRematchDefeated
	msgbox Route109_Text_LolaRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Carmen:: @ 814F885
	trainerbattle_single TRAINER_CARMEN, Route109_Text_CarmenIntro, Route109_Text_CarmenDefeated
	msgbox Route109_Text_CarmenPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Gwen:: @ 814F89C
	trainerbattle_single TRAINER_GWEN, Route109_Text_GwenIntro, Route109_Text_GwenDefeated
	msgbox Route109_Text_GwenPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Carter:: @ 814F8B3
	trainerbattle_single TRAINER_CARTER, Route109_Text_CarterIntro, Route109_Text_CarterDefeated
	msgbox Route109_Text_CarterPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Paul:: @ 814F8CA
	trainerbattle_double TRAINER_MEL_AND_PAUL, Route109_Text_PaulIntro, Route109_Text_PaulDefeated, Route109_Text_PaulNotEnoughPokemon
	msgbox Route109_Text_PaulPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Mel:: @ 814F8E5
	trainerbattle_double TRAINER_MEL_AND_PAUL, Route109_Text_MelIntro, Route109_Text_MelDefeated, Route109_Text_MelNotEnoughPokemon
	msgbox Route109_Text_MelPostBattle, MSGBOX_AUTOCLOSE
	end
