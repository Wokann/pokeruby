Route119_MapScripts:: @ 8151167
	map_script MAP_SCRIPT_ON_RESUME, Route119_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, Route119_OnTransition
	.byte 0

Route119_OnResume:: @ 8151172
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, Route119_EventScript_TryRemoveKecleon
	end

Route119_EventScript_TryRemoveKecleon:: @ 815117C
	compare VAR_0x8004, 6
	call_if_eq Route119_EventScript_RemoveKecleon1
	compare VAR_0x8004, 7
	call_if_eq Route119_EventScript_RemoveKecleon2
	return

Route119_EventScript_RemoveKecleon1:: @ 8151193
	removeobject 35
	return

Route119_EventScript_RemoveKecleon2:: @ 8151197
	removeobject 36
	return

Route119_OnTransition:: @ 815119B
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupRivalOnBikeGfxId
	call Common_EventScript_SetupEvilTeamGfxIds
	compare VAR_WEATHER_INSTITUTE_STATE, 1
	call_if_eq Route119_EventScript_MoveInstituteWorkersDownstairs
	special SetRoute119Weather
	end

Route119_EventScript_MoveInstituteWorkersDownstairs:: @ 81511B9
	setflag FLAG_HIDE_WEATHER_INSTITUTE_WORKERS_2F
	clearflag FLAG_HIDE_WEATHER_INSTITUTE_WORKERS_1F
	setvar VAR_WEATHER_INSTITUTE_STATE, 2
	return

Route119_EventScript_RivalTrigger1:: @ 81511C5
	setvar VAR_TEMP_1, 1
	goto Route119_EventScript_RivalEncounter
	end

Route119_EventScript_RivalTrigger2:: @ 81511D0
	setvar VAR_TEMP_1, 2
	goto Route119_EventScript_RivalEncounter
	end

Route119_EventScript_RivalEncounter:: @ 81511DB
	lockall
	addobject 25
	checkplayergender
	compare VAR_RESULT, 0
	call_if_eq Route119_EventScript_PlayMayMusic
	compare VAR_RESULT, 1
	call_if_eq Route119_EventScript_PlayBrendanMusic
	delay 65
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_RivalEnter1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_RivalEnter2
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 30
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_SetRivalPos1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_SetRivalPos2
	removeobject 25
	addobject 16
	delay 30
	checkplayergender
	compare VAR_RESULT, 0
	goto_if_eq Route119_EventScript_BattleMay
	compare VAR_RESULT, 1
	goto_if_eq Route119_EventScript_BattleBrendan
	releaseall
	end

Route119_EventScript_PlayMayMusic:: @ 8151254
	playbgm MUS_ENCOUNTER_MAY, TRUE
	return

Route119_EventScript_PlayBrendanMusic:: @ 8151259
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	return

Route119_EventScript_BattleMay:: @ 815125E
	msgbox Route119_Text_MayIntro, MSGBOX_DEFAULT
	switch VAR_STARTER_MON
	case 0, Route119_EventScript_BattleMayTreecko
	case 1, Route119_EventScript_BattleMayTorchic
	case 2, Route119_EventScript_BattleMayMudkip
	end

Route119_EventScript_BattleMayTreecko:: @ 815128D
	trainerbattle_no_intro TRAINER_MAY_6, Route119_Text_MayDefeat
	goto Route119_EventScript_DefeatedMay
	end

Route119_EventScript_BattleMayTorchic:: @ 815129D
	trainerbattle_no_intro TRAINER_MAY_9, Route119_Text_MayDefeat
	goto Route119_EventScript_DefeatedMay
	end

Route119_EventScript_BattleMayMudkip:: @ 81512AD
	trainerbattle_no_intro TRAINER_MAY_3, Route119_Text_MayDefeat
	goto Route119_EventScript_DefeatedMay
	end

Route119_EventScript_DefeatedMay:: @ 81512BD
	msgbox Route119_Text_MayPresentForYou, MSGBOX_DEFAULT
	call Route119_EventScript_GiveFlyHM
	msgbox Route119_Text_MayExplainFly, MSGBOX_DEFAULT
	goto Route119_EventScript_RivalExit
	end

Route119_EventScript_BattleBrendan:: @ 81512D8
	msgbox Route119_Text_BrendanIntro, MSGBOX_DEFAULT
	switch VAR_STARTER_MON
	case 0, Route119_EventScript_BattleBrendanTreecko
	case 1, Route119_EventScript_BattleBrendanTorchic
	case 2, Route119_EventScript_BattleBrendanMudkip
	end

Route119_EventScript_BattleBrendanTreecko:: @ 8151307
	trainerbattle_no_intro TRAINER_BRENDAN_6, Route119_Text_BrendanDefeat
	goto Route119_EventScript_DefeatedBrendan
	end

Route119_EventScript_BattleBrendanTorchic:: @ 8151317
	trainerbattle_no_intro TRAINER_BRENDAN_9, Route119_Text_BrendanDefeat
	goto Route119_EventScript_DefeatedBrendan
	end

Route119_EventScript_BattleBrendanMudkip:: @ 8151327
	trainerbattle_no_intro TRAINER_BRENDAN_3, Route119_Text_BrendanDefeat
	goto Route119_EventScript_DefeatedBrendan
	end

Route119_EventScript_DefeatedBrendan:: @ 8151337
	msgbox Route119_Text_BrendanIllGiveYouThis, MSGBOX_DEFAULT
	call Route119_EventScript_GiveFlyHM
	msgbox Route119_Text_BrendanExplainFly, MSGBOX_DEFAULT
	goto Route119_EventScript_RivalExit
	end

Route119_EventScript_GiveFlyHM:: @ 8151352
	giveitem ITEM_HM02_FLY
	setflag FLAG_RECEIVED_HM02
	return

Route119_EventScript_RivalExit:: @ 8151362
	closemessage
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_SetRivalPos1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_SetRivalPos2
	removeobject 16
	addobject 25
	delay 30
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_RivalExit1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_RivalExit2
	removeobject 25
	setvar VAR_ROUTE119_STATE, 1
	savebgm 0
	fadedefaultbgm
	releaseall
	end

Route119_EventScript_RivalEnter1:: @ 81513A6
	applymovement 25, Route119_Movement_RivalEnter1
	waitmovement 0
	return

Route119_EventScript_RivalEnter2:: @ 81513B1
	applymovement 25, Route119_Movement_RivalEnter2
	waitmovement 0
	return

Route119_EventScript_RivalExit1:: @ 81513BC
	applymovement 25, Route119_Movement_RivalExit1
	waitmovement 0
	return

Route119_EventScript_RivalExit2:: @ 81513C7
	applymovement 25, Route119_Movement_RivalExit2
	waitmovement 0
	return

Route119_EventScript_SetRivalPos1:: @ 81513D2
	setobjectxyperm 16, 25, 32
	setobjectxyperm 25, 25, 32
	return

Route119_EventScript_SetRivalPos2:: @ 81513E1
	setobjectxyperm 16, 26, 32
	setobjectxyperm 25, 26, 32
	return

Route119_Movement_RivalEnter1:: @ 81513F0
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	step_end

Route119_Movement_RivalEnter2:: @ 81513FA
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	step_end

Route119_Movement_RivalExit1:: @ 8151405
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route119_Movement_RivalExit2:: @ 815140F
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route119_EventScript_CyclingTriathleteM:: @ 815141A
	msgbox Route119_Text_TallGrassSnaresBikeTires, MSGBOX_NPC
	end

Route119_EventScript_RouteSignFortree:: @ 8151423
	msgbox Route119_Text_RouteSignFortree, MSGBOX_SIGN
	end

Route119_EventScript_WeatherInstituteSign:: @ 815142C
	msgbox Route119_Text_WeatherInstitute, MSGBOX_SIGN
	end

Route119_EventScript_Brent:: @ 8151435
	trainerbattle_single TRAINER_BRENT, Route119_Text_BrentIntro, Route119_Text_BrentDefeat
	msgbox Route119_Text_BrentPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Donald:: @ 815144C
	trainerbattle_single TRAINER_DONALD, Route119_Text_DonaldIntro, Route119_Text_DonaldDefeat
	msgbox Route119_Text_DonaldPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Taylor:: @ 8151463
	trainerbattle_single TRAINER_TAYLOR, Route119_Text_TaylorIntro, Route119_Text_TaylorDefeat
	msgbox Route119_Text_TaylorPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Doug:: @ 815147A
	trainerbattle_single TRAINER_DOUG, Route119_Text_DougIntro, Route119_Text_DougDefeat
	msgbox Route119_Text_DougPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Greg:: @ 8151491
	trainerbattle_single TRAINER_GREG, Route119_Text_GregIntro, Route119_Text_GregDefeat
	msgbox Route119_Text_GregPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Kent:: @ 81514A8
	trainerbattle_single TRAINER_KENT, Route119_Text_KentIntro, Route119_Text_KentDefeat
	msgbox Route119_Text_KentPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Jackson:: @ 81514BF
	trainerbattle_single TRAINER_JACKSON_1, Route119_Text_JacksonIntro, Route119_Text_JacksonDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route119_EventScript_RematchJackson
	msgbox Route119_Text_JacksonPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_RematchJackson:: @ 81514E6
	trainerbattle_rematch TRAINER_JACKSON_1, Route119_Text_JacksonRematchIntro, Route119_Text_JacksonRematchDefeat
	msgbox Route119_Text_JacksonPostRematch, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Catherine:: @ 81514FD
	trainerbattle_single TRAINER_CATHERINE_1, Route119_Text_CatherineIntro, Route119_Text_CatherineDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route119_EventScript_RematchCatherine
	msgbox Route119_Text_CatherinePostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_RematchCatherine:: @ 8151524
	trainerbattle_rematch TRAINER_CATHERINE_1, Route119_Text_CatherineRematchIntro, Route119_Text_CatherineRematchDefeat
	msgbox Route119_Text_CatherinePostRematch, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Hugh:: @ 815153B
	trainerbattle_single TRAINER_HUGH, Route119_Text_HughIntro, Route119_Text_HughDefeat
	msgbox Route119_Text_HughPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Phil:: @ 8151552
	trainerbattle_single TRAINER_PHIL, Route119_Text_PhilIntro, Route119_Text_PhilDefeat
	msgbox Route119_Text_PhilPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Yasu:: @ 8151569
	trainerbattle_single TRAINER_YASU, Route119_Text_YasuIntro, Route119_Text_YasuDefeat
	msgbox Route119_Text_YasuPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Takashi:: @ 8151580
	trainerbattle_single TRAINER_TAKASHI, Route119_Text_TakashiIntro, Route119_Text_TakashiDefeat
	msgbox Route119_Text_TakashiPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Hideo:: @ 8151597
	trainerbattle_single TRAINER_HIDEO_2, Route119_Text_HideoIntro, Route119_Text_HideoDefeat
	msgbox Route119_Text_HideoPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Eugene:: @ 81515AE
	trainerbattle_single TRAINER_EUGENE, Route119_Text_EugeneIntro, Route119_Text_EugeneDefeat
	msgbox Route119_Text_EugenePostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_BridgeAquaGrunt1:: @ 81515C5
	lock
	faceplayer
	msgbox Route119_Text_StayAwayFromWeatherInstitute, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route119_EventScript_BridgeAquaGrunt2:: @ 81515DC
	lock
	faceplayer
	msgbox Route119_Text_DontGoNearWeatherInstitute, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route119_EventScript_Boy1:: @ 81515F3
	msgbox Route119_Text_ThoughtFlyByCatchingBirdMons, MSGBOX_NPC
	end

Route119_EventScript_Boy2:: @ 81515FC
	msgbox Route119_Text_CanYourMonMakeSecretBase, MSGBOX_NPC
	end

Route119_EventScript_TrainerTipsDecoration:: @ 8151605
	msgbox Route119_Text_TrainerTipsDecoration, MSGBOX_SIGN
	end
