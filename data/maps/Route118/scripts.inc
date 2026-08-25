Route118_MapScripts:: @ 8150F28
	map_script MAP_SCRIPT_ON_TRANSITION, Route118_OnTransition
	.byte 0

Route118_OnTransition:: @ 8150F2E
	call Route118_EventScript_1AE313
	end

Route118_EventScript_GoodRodFisherman:: @ 8150F34
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_GOOD_ROD, Route118_EventScript_ReceivedGoodRod
	msgbox Route118_Text_YouAgreeGoodRodIsGood, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route118_EventScript_ReceiveGoodRod
	compare VAR_RESULT, NO
	goto_if_eq Route118_EventScript_DeclineGoodRod
	end

Route118_EventScript_ReceiveGoodRod:: @ 8150F5E
	msgbox Route118_Text_IdenticalMindsTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_GOOD_ROD
	setflag FLAG_RECEIVED_GOOD_ROD
	msgbox Route118_Text_TryYourLuckFishing, MSGBOX_DEFAULT
	release
	end

Route118_EventScript_DeclineGoodRod:: @ 8150F7F
	msgbox Route118_Text_DontYouLikeToFish, MSGBOX_DEFAULT
	release
	end

Route118_EventScript_ReceivedGoodRod:: @ 8150F89
	msgbox Route118_Text_TryCatchingMonWithGoodRod, MSGBOX_DEFAULT
	release
	end

Route118_EventScript_Girl:: @ 8150F93
	msgbox Route118_Text_CanCrossRiversWithSurf, MSGBOX_NPC
	end

Route118_EventScript_RouteSignMauville:: @ 8150F9C
	msgbox Route118_Text_RouteSignMauville, MSGBOX_SIGN
	end

Route118_EventScript_RouteSign119:: @ 8150FA5
	msgbox Route118_Text_RouteSign119, MSGBOX_SIGN
	end

Route118_EventScript_StevenTrigger0:: @ 8150FAE
	lockall
	setvar VAR_0x8008, 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	applymovement 19, Route118_Movement_StevenApproachLedge0
	waitmovement 0
	goto Route118_EventScript_StevenTrigger
	end

Route118_EventScript_StevenTrigger1:: @ 8150FCE
	lockall
	setvar VAR_0x8008, 1
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	goto Route118_EventScript_StevenTrigger
	end

Route118_EventScript_StevenTrigger2:: @ 8150FE4
	lockall
	setvar VAR_0x8008, 2
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	applymovement 19, Route118_Movement_StevenApproachLedge2
	waitmovement 0
	goto Route118_EventScript_StevenTrigger
	end

Route118_EventScript_StevenTrigger:: @ 8151004
	playse SE_LEDGE
	applymovement 19, Route118_Movement_StevenJumpLedge
	waitmovement 0
	delay 30
	msgbox Route118_Text_StevenQuestions, MSGBOX_DEFAULT
	closemessage
	compare VAR_0x8008, 0
	call_if_eq Route118_EventScript_StevenExit0
	compare VAR_0x8008, 1
	call_if_eq Route118_EventScript_StevenExit1
	compare VAR_0x8008, 2
	call_if_eq Route118_EventScript_StevenExit2
	setvar VAR_ROUTE118_STATE, 1
	removeobject 19
	releaseall
	end

Route118_EventScript_StevenExit0:: @ 8151048
	applymovement 19, Route118_Movement_StevenExit0
	waitmovement 0
	return

Route118_EventScript_StevenExit1:: @ 8151053
	applymovement 19, Route118_Movement_StevenExit1
	waitmovement 0
	return

Route118_EventScript_StevenExit2:: @ 815105E
	applymovement 19, Route118_Movement_StevenExit2
	waitmovement 0
	return

Route118_Movement_StevenApproachLedge0:: @ 8151069
	walk_left
	step_end

Route118_Movement_StevenApproachLedge2:: @ 815106B
	walk_right
	step_end

Route118_Movement_StevenJumpLedge:: @ 815106D
	jump_2_down
	delay_16
	walk_down
	step_end

Route118_Movement_StevenExit0:: @ 8151071
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route118_Movement_StevenExit1:: @ 815107C
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route118_Movement_StevenExit2:: @ 8151086
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route118_EventScript_Rose:: @ 815108F
	trainerbattle_single TRAINER_ROSE_1, Route118_Text_RoseIntro, Route118_Text_RoseDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route118_EventScript_RematchRose
	msgbox Route118_Text_RosePostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_RematchRose:: @ 81510B6
	trainerbattle_rematch TRAINER_ROSE_1, Route118_Text_RoseRematchIntro, Route118_Text_RoseRematchDefeat
	msgbox Route118_Text_RosePostRematch, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Barny:: @ 81510CD
	trainerbattle_single TRAINER_BARNY, Route118_Text_BarnyIntro, Route118_Text_BarnyDefeat
	msgbox Route118_Text_BarnyPostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Wade:: @ 81510E4
	trainerbattle_single TRAINER_WADE, Route118_Text_WadeIntro, Route118_Text_WadeDefeat
	msgbox Route118_Text_WadePostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Dalton:: @ 81510FB
	trainerbattle_single TRAINER_DALTON_1, Route118_Text_DaltonIntro, Route118_Text_DaltonDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route118_EventScript_RematchDalton
	msgbox Route118_Text_DaltonPostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_RematchDalton:: @ 8151122
	trainerbattle_rematch TRAINER_DALTON_1, Route118_Text_DaltonRematchIntro, Route118_Text_DaltonRematchDefeat
	msgbox Route118_Text_DaltonPostRematch, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Perry:: @ 8151139
	trainerbattle_single TRAINER_PERRY, Route118_Text_PerryIntro, Route118_Text_PerryDefeat
	msgbox Route118_Text_PerryPostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Chester:: @ 8151150
	trainerbattle_single TRAINER_CHESTER, Route118_Text_ChesterIntro, Route118_Text_ChesterDefeat
	msgbox Route118_Text_ChesterPostBattle, MSGBOX_AUTOCLOSE
	end
