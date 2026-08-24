Route111_MapScripts:: @ 814FF91
	map_script MAP_SCRIPT_ON_LOAD, Route111_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Route111_OnTransition
	.byte 0

Route111_OnLoad:: @ 814FF9C
	call_if_unset FLAG_REGI_DOORS_OPENED, Route111_EventScript_CloseDesertRuins
	end

Route111_EventScript_CloseDesertRuins:: @ 814FFA6
	setmetatile 29, 86, METATILE_General_RockWall_RockBase, 1
	setmetatile 29, 87, METATILE_General_RockWall_SandBase, 1
	return

Route111_OnTransition:: @ 814FFB9
	call Route111_EventScript_CheckSetSandstorm
	call Route111_EventScript_1AE313
	goto_if_not_defeated TRAINER_VICKY, Route111_EventScript_SetWinstratesNotDefeated
	end

Route111_EventScript_CheckSetSandstorm:: @ 814FFCD
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	compare VAR_TEMP_1, 34
	goto_if_lt Route111_EventScript_EndCheckSetSandstorm
	compare VAR_TEMP_1, 107
	goto_if_gt Route111_EventScript_EndCheckSetSandstorm
	compare VAR_TEMP_1, 72
	goto_if_gt Route111_EventScript_SetSandstorm
	compare VAR_TEMP_0, 2000
	goto_if_gt Route111_EventScript_EndCheckSetSandstorm
	compare VAR_TEMP_0, 8
	goto_if_lt Route111_EventScript_EndCheckSetSandstorm

Route111_EventScript_SetSandstorm:: @ 8150009
	setweather WEATHER_SANDSTORM

Route111_EventScript_EndCheckSetSandstorm:: @ 815000C
	return

Route111_EventScript_SetWinstratesNotDefeated:: @ 815000D
	clearflag FLAG_HIDE_VICTOR_WINSTRATE
	setflag FLAG_HIDE_VICTORIA_WINSTRATE
	setflag FLAG_HIDE_VIVI_WINSTRATE
	setflag FLAG_HIDE_VICKI_WINSTRATE
	cleartrainerflag TRAINER_VICTOR
	cleartrainerflag TRAINER_VICTORIA
	cleartrainerflag TRAINER_VIVI
	end

Route111_EventScript_RootFossil:: @ 8150023
	lockall
	msgbox Route111_Text_RootFossil, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route111_EventScript_LeaveRootFossil
	giveitem ITEM_ROOT_FOSSIL
	closemessage
	setflag FLAG_HIDE_ROOT_FOSSIL
	setflag FLAG_HIDE_CLAW_FOSSIL
	removeobject 34
	delay 30
	applymovement 35, Route111_Movement_FossilDisappear
	waitmovement 0
	removeobject 35
	releaseall
	end

Route111_EventScript_LeaveRootFossil:: @ 815005F
	msgbox Route111_Text_LeaveRootFossil, MSGBOX_DEFAULT
	releaseall
	end

Route111_EventScript_ClawFossil:: @ 8150069
	lockall
	msgbox Route111_Text_ClawFossil, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route111_EventScript_LeaveClawFossil
	giveitem ITEM_CLAW_FOSSIL
	closemessage
	setflag FLAG_HIDE_CLAW_FOSSIL
	setflag FLAG_HIDE_ROOT_FOSSIL
	removeobject 35
	delay 30
	applymovement 34, Route111_Movement_FossilDisappear
	waitmovement 0
	removeobject 34
	releaseall
	end

Route111_EventScript_LeaveClawFossil:: @ 81500A5
	msgbox Route111_Text_LeaveClawFossil, MSGBOX_DEFAULT
	releaseall
	end

Route111_Movement_FossilDisappear:: @ 81500AF
	set_visible
	delay_4
	set_invisible
	delay_4
	set_visible
	delay_4
	set_invisible
	delay_4
	set_visible
	delay_8
	set_invisible
	delay_8
	set_visible
	delay_8
	set_invisible
	delay_8
	set_visible
	delay_16
	set_invisible
	delay_16
	set_visible
	step_end

Route111_EventScript_Girl:: @ 81500C5
	lock
	faceplayer
	dotimebasedevents
	goto_if_set FLAG_DAILY_RECEIVED_BERRY_ROUTE111, Route111_EventScript_ReceivedBerry
	msgbox Route111_Text_WateredPlantsEveryDayTakeBerry, MSGBOX_DEFAULT
	giveitem ITEM_RAZZ_BERRY
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_DAILY_RECEIVED_BERRY_ROUTE111
	special GetPlayerBigGuyGirlString
	msgbox Route111_Text_GoingToTryToMakeDifferentColorBerries, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_ReceivedBerry:: @ 8150100
	msgbox Route111_Text_WhatColorBerriesToLookForToday, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_ViciousSandstormTriggerUp:: @ 815010A
	lockall
	setvar VAR_0x8004, 0
	goto Route111_EventScript_ViciousSandstormTrigger
	end

Route111_EventScript_ViciousSandstormTriggerDown:: @ 8150116
	lockall
	setvar VAR_0x8004, 1
	goto Route111_EventScript_ViciousSandstormTrigger
	end

Route111_EventScript_ViciousSandstormTriggerLeft:: @ 8150122
	lockall
	setvar VAR_0x8004, 2
	goto Route111_EventScript_ViciousSandstormTrigger
	end

Route111_EventScript_ViciousSandstormTriggerRight:: @ 815012E
	lockall
	setvar VAR_0x8004, 3
	goto Route111_EventScript_ViciousSandstormTrigger
	end

Route111_EventScript_ViciousSandstormTrigger:: @ 815013A
	checkitem ITEM_GO_GOGGLES, 1
	compare VAR_RESULT, 0
	goto_if_eq Route111_EventScript_PreventRouteAccess
	setvar VAR_TEMP_3, 1
	releaseall
	end

Route111_EventScript_PreventRouteAccess:: @ 8150151
	msgbox Route111_Text_1A0F93, MSGBOX_DEFAULT
	closemessage
	compare VAR_0x8004, 0
	call_if_eq Route111_EventScript_PushUpFromRoute
	compare VAR_0x8004, 1
	call_if_eq Route111_EventScript_PushDownFromRoute
	compare VAR_0x8004, 2
	call_if_eq Route111_EventScript_PushLeftFromRoute
	compare VAR_0x8004, 3
	call_if_eq Route111_EventScript_PushRightFromRoute
	releaseall
	end

Route111_EventScript_PushUpFromRoute:: @ 8150188
	applymovement LOCALID_PLAYER, Route111_Movement_PushUpFromRoute
	waitmovement 0
	return

Route111_EventScript_PushDownFromRoute:: @ 8150193
	applymovement LOCALID_PLAYER, Route111_Movement_PushDownFromRoute
	waitmovement 0
	return

Route111_EventScript_PushLeftFromRoute:: @ 815019E
	applymovement LOCALID_PLAYER, Route111_Movement_PushLeftFromRoute
	waitmovement 0
	return

Route111_EventScript_PushRightFromRoute:: @ 81501A9
	applymovement LOCALID_PLAYER, Route111_Movement_PushRightFromRoute
	waitmovement 0
	return

Route111_Movement_PushUpFromRoute:: @ 81501B4
	walk_up
	step_end

Route111_Movement_PushDownFromRoute:: @ 81501B6
	walk_down
	step_end

Route111_Movement_PushLeftFromRoute:: @ 81501B8
	walk_left
	step_end

Route111_Movement_PushRightFromRoute:: @ 81501BA
	walk_right
	step_end

Route111_EventScript_SunTrigger:: @ 81501BC
	setweather WEATHER_SUNNY
	fadenewbgm MUS_ROUTE110
	doweather
	setvar VAR_TEMP_3, 0
	end

Route111_EventScript_SandstormTrigger:: @ 81501C9
	setweather WEATHER_SANDSTORM
	fadenewbgm MUS_ROUTE111
	doweather
	end

Route111_EventScript_Victor:: @ 81501D1
	lock
	faceplayer
	setflag FLAG_LANDMARK_WINSTRATE_FAMILY
	msgbox Route111_Text_BattleOurFamily, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route111_EventScript_BattleWinstrates
	msgbox Route111_Text_IsThatSo, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_BattleWinstrates:: @ 81501F3
	msgbox Route111_Text_VictorIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VICTOR, Route111_Text_VictorDefeat
	applymovement 1, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	msgbox Route111_Text_VictorPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement 1, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject 1
	call Route111_EventScript_CloseWinstrateDoor
	applymovement LOCALID_PLAYER, Route111_Movement_WaitForNextWinstrate
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	addobject 2
	applymovement 2, Route111_Movement_WinstrateExitHouse
	waitmovement 0
	call Route111_EventScript_CloseWinstrateDoor
	msgbox Route111_Text_VictoriaIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VICTORIA, Route111_Text_VictoriaDefeat
	applymovement 2, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	msgbox Route111_Text_VictoriaPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement 2, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject 2
	call Route111_EventScript_CloseWinstrateDoor
	applymovement LOCALID_PLAYER, Route111_Movement_WaitForNextWinstrate
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	addobject 3
	applymovement 3, Route111_Movement_WinstrateExitHouse
	waitmovement 0
	call Route111_EventScript_CloseWinstrateDoor
	msgbox Route111_Text_ViviIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VIVI, Route111_Text_ViviDefeat
	applymovement 3, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	msgbox Route111_Text_ViviPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement 3, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject 3
	call Route111_EventScript_CloseWinstrateDoor
	applymovement LOCALID_PLAYER, Route111_Movement_WaitForNextWinstrate
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	addobject 4
	applymovement 4, Route111_Movement_WinstrateExitHouse
	waitmovement 0
	call Route111_EventScript_CloseWinstrateDoor
	msgbox Route111_Text_VickyIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VICKY, Route111_Text_VickyDefeat
	msgbox Route111_Text_VickyPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement 4, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	applymovement 4, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject 4
	call Route111_EventScript_CloseWinstrateDoor
	release
	end

Route111_EventScript_OpenWinstrateDoor:: @ 8150348
	opendoor 13, 113
	waitdooranim
	return

Route111_EventScript_CloseWinstrateDoor:: @ 815034F
	closedoor 13, 113
	waitdooranim
	return

Route111_Movement_WinstrateEnterHouse:: @ 8150356
	walk_in_place_fastest_up
	walk_up
	step_end

Route111_Movement_WinstrateExitHouse:: @ 8150359
	walk_down
	step_end

Route111_Movement_WaitForNextWinstrate:: @ 815035B
	delay_16
	delay_16
	delay_16
	step_end

Route111_EventScript_RouteSignMauville:: @ 815035F
	msgbox Route111_Text_RouteSignMauville, MSGBOX_SIGN
	end

Route111_EventScript_WinstrateHouseSign:: @ 8150368
	msgbox Route111_Text_WinstrateHouseSign, MSGBOX_SIGN
	end

Route111_EventScript_RouteSign112:: @ 8150371
	msgbox Route111_Text_RouteSign112, MSGBOX_SIGN
	end

Route111_EventScript_RouteSign113:: @ 815037A
	msgbox Route111_Text_RouteSign113, MSGBOX_SIGN
	end

Route111_EventScript_OldLadysRestStopSign:: @ 8150383
	msgbox Route111_Text_OldLadysRestStopSign, MSGBOX_SIGN
	end

Route111_EventScript_TrainerTipsSpAtkSpDef:: @ 815038C
	msgbox Route111_Text_TrainerTipsSpAtkSpDef, MSGBOX_SIGN
	end

Route111_EventScript_Man1:: @ 8150395
	msgbox Route111_Text_ToughToKeepWinningUpTheRanks, MSGBOX_NPC
	end

Route111_EventScript_Man2:: @ 815039E
	msgbox Route111_Text_WinstrateFamilyDestroyedMe, MSGBOX_NPC
	end

Route111_EventScript_Drew:: @ 81503A7
	trainerbattle_single TRAINER_DREW, Route111_Text_DrewIntro, Route111_Text_DrewDefeat
	msgbox Route111_Text_DrewPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Heidi:: @ 81503BE
	trainerbattle_single TRAINER_HEIDI, Route111_Text_HeidiIntro, Route111_Text_HeidiDefeat
	msgbox Route111_Text_HeidiPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Cliff:: @ 81503D5
	trainerbattle_single TRAINER_CLIFF, Route111_Text_CliffIntro, Route111_Text_CliffDefeat
	msgbox Route111_Text_CliffPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Becky:: @ 81503EC
	trainerbattle_single TRAINER_BECKY, Route111_Text_BeckyIntro, Route111_Text_BeckyDefeat
	msgbox Route111_Text_BeckyPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Dusty:: @ 8150403
	trainerbattle_single TRAINER_DUSTY_1, Route111_Text_DustyIntro, Route111_Text_DustyDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route111_EventScript_RematchDusty
	msgbox Route111_Text_DustyPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_RematchDusty:: @ 815042A
	trainerbattle_rematch TRAINER_DUSTY_1, Route111_Text_DustyRematchIntro, Route111_Text_DustyRematchDefeat
	msgbox Route111_Text_DustyPostRematch, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Travis:: @ 8150441
	trainerbattle_single TRAINER_TRAVIS, Route111_Text_TravisIntro, Route111_Text_TravisDefeat
	msgbox Route111_Text_TravisPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Irene:: @ 8150458
	trainerbattle_single TRAINER_IRENE, Route111_Text_IreneIntro, Route111_Text_IreneDefeat
	msgbox Route111_Text_IrenePostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Daisuke:: @ 815046F
	trainerbattle_single TRAINER_DAISUKE, Route111_Text_DaisukeIntro, Route111_Text_DaisukeDefeat
	msgbox Route111_Text_DaisukePostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Wilton:: @ 8150486
	trainerbattle_single TRAINER_WILTON_1, Route111_Text_WiltonIntro, Route111_Text_WiltonDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route111_EventScript_RematchWilton
	msgbox Route111_Text_WiltonPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_RematchWilton:: @ 81504AD
	trainerbattle_rematch TRAINER_WILTON_1, Route111_Text_WiltonRematchIntro, Route111_Text_WiltonRematchDefeat
	msgbox Route111_Text_WiltonPostRematch, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Brooke:: @ 81504C4
	trainerbattle_single TRAINER_BROOKE_1, Route111_Text_BrookeIntro, Route111_Text_BrookeDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route111_EventScript_RematchBrooke
	msgbox Route111_Text_BrookePostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_RematchBrooke:: @ 81504EB
	trainerbattle_rematch TRAINER_BROOKE_1, Route111_Text_BrookeRematchIntro, Route111_Text_BrookeRematchDefeat
	msgbox Route111_Text_BrookePostRematch, MSGBOX_AUTOCLOSE
	end
