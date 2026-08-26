Route128_MapScripts:: @ 815211B
	map_script MAP_SCRIPT_ON_TRANSITION, Route128_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route128_OnFrame
	.byte 0

Route128_OnTransition:: @ 8152126
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetLegendaryWeather
	call Common_EventScript_SetupEvilTeamGfxIds
	end

Route128_OnFrame:: @ 8152135
	map_script_2 VAR_ROUTE128_STATE, 1, Route128_EventScript_LegendaryAwakenedScene
	.2byte 0

Route128_EventScript_LegendaryAwakenedScene:: @ 815213F
	lockall
	delay 20
	.ifndef SAPPHIRE
	special DoGroudonDroughtFlash
	waitstate
	.endif
	applymovement LOCALID_ROUTE128_EVIL_LEADER, Route128_Movement_EvilLeaderLookAround
	waitmovement 0
	msgbox Route128_Text_EvilLeaderWhatHappened, MSGBOX_DEFAULT
	applymovement LOCALID_ROUTE128_EVIL_LEADER, Route128_Movement_EvilLeaderBackUp
	waitmovement 0
	msgbox Route128_Text_EvilLeaderIOnlyWanted, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE128_GOOD_LEADER, Route128_Movement_GoodLeaderApproachEvilLeader
	waitmovement 0
	applymovement LOCALID_ROUTE128_EVIL_LEADER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	msgbox Route128_Text_GoodLeaderDoYouUnderstandNow, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE128_GOOD_LEADER, Route128_Movement_GoodLeaderApproachPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	msgbox Route128_Text_GoodLeaderLeaveThingsToUs, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE128_EVIL_LEADER, Route128_Movement_EvilLeaderRunLeft
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	applymovement LOCALID_ROUTE128_GOOD_LEADER, Route128_Movement_GoodLeaderWalkLeft
	waitmovement 0
	msgbox Route128_Text_GoodLeaderThisDefiesBelief, MSGBOX_DEFAULT
	closemessage
	delay 40
	applymovement LOCALID_ROUTE128_GOOD_LEADER, Route128_Movement_GoodLeaderExit
	applymovement LOCALID_ROUTE128_EVIL_LEADER, Route128_Movement_EvilLeaderExit
	waitmovement 0
	removeobject LOCALID_ROUTE128_GOOD_LEADER
	removeobject LOCALID_ROUTE128_EVIL_LEADER
	delay 100
	setfieldeffectargument 0, 1
	dofieldeffect FLDEFF_NPCFLY_OUT
	waitfieldeffect FLDEFF_NPCFLY_OUT
	addobject LOCALID_ROUTE128_STEVEN
	applymovement LOCALID_ROUTE128_STEVEN, Route128_Movement_StevenApproachPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	msgbox Route128_Text_StevenWhatIsHappening, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE128_STEVEN, Route128_Movement_StevenWalkUp
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	.ifdef SAPPHIRE
	msgbox Route128_Text_StevenWholeWorldWillDrown, MSGBOX_DEFAULT
	.else
	msgbox Route128_Text_StevenWholeWorldWillBeParched, MSGBOX_DEFAULT
	.endif
	applymovement LOCALID_ROUTE128_STEVEN, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	msgbox Route128_Text_StevenImGoingToSootopolis, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE128_STEVEN, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 50
	setfieldeffectargument 0, 1
	dofieldeffect FLDEFF_NPCFLY_OUT
	delay 15
	removeobject LOCALID_ROUTE128_STEVEN
	waitfieldeffect FLDEFF_NPCFLY_OUT
	setvar VAR_ROUTE128_STATE, 2
	releaseall
	end

Route128_Movement_Unused1:: @ 815225E
	walk_fast_left
	step_end

Route128_Movement_Unused2:: @ 8152260
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

Route128_Movement_StevenWalkUp:: @ 815226D
	walk_up
	step_end

Route128_Movement_StevenApproachPlayer:: @ 815226F
	walk_fast_left
	step_end

Route128_Movement_EvilLeaderLookAround:: @ 8152271
	walk_fast_down
	walk_in_place_fastest_left
	delay_16
	walk_in_place_fastest_right
	delay_16
	delay_16
	walk_in_place_fastest_down
	step_end

Route128_Movement_EvilLeaderBackUp:: @ 8152279
	lock_facing_direction
	walk_slow_up
	delay_16
	unlock_facing_direction
	step_end

Route128_Movement_EvilLeaderRunLeft:: @ 815227E
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_in_place_fastest_right
	step_end

Route128_Movement_EvilLeaderExit:: @ 8152283
	delay_16
	delay_16
	walk_fast_up
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route128_Movement_GoodLeaderWalkLeft:: @ 8152290
	walk_left
	walk_left
	step_end

Route128_Movement_GoodLeaderApproachEvilLeader:: @ 8152293
	walk_left
	walk_in_place_fastest_down
	step_end

Route128_Movement_GoodLeaderExit:: @ 8152296
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route128_Movement_GoodLeaderApproachPlayer:: @ 81522A1
	walk_right
	walk_in_place_fastest_down
	step_end

Route128_EventScript_Isaiah:: @ 81522A4
	trainerbattle_single TRAINER_ISAIAH_1, Route128_Text_IsaiahIntro, Route128_Text_IsaiahDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route128_EventScript_RematchIsaiah
	msgbox Route128_Text_IsaiahPostBattle, MSGBOX_AUTOCLOSE
	end

Route128_EventScript_RematchIsaiah:: @ 81522CB
	trainerbattle_rematch TRAINER_ISAIAH_1, Route128_Text_IsaiahRematchIntro, Route128_Text_IsaiahRematchDefeat
	msgbox Route128_Text_IsaiahPostRematch, MSGBOX_AUTOCLOSE
	end

Route128_EventScript_Katelyn:: @ 81522E2
	trainerbattle_single TRAINER_KATELYN_1, Route128_Text_KatelynIntro, Route128_Text_KatelynDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route128_EventScript_RematchKatelyn
	msgbox Route128_Text_KatelynPostBattle, MSGBOX_AUTOCLOSE
	end

Route128_EventScript_RematchKatelyn:: @ 8152309
	trainerbattle_rematch TRAINER_KATELYN_1, Route128_Text_KatelynRematchIntro, Route128_Text_KatelynRematchDefeat
	msgbox Route128_Text_KatelynPostRematch, MSGBOX_AUTOCLOSE
	end

Route128_EventScript_Alexa:: @ 8152320
	trainerbattle_single TRAINER_ALEXA, Route128_Text_AlexaIntro, Route128_Text_AlexaDefeat
	msgbox Route128_Text_AlexaPostBattle, MSGBOX_AUTOCLOSE
	end

Route128_EventScript_Ruben:: @ 8152337
	trainerbattle_single TRAINER_RUBEN, Route128_Text_RubenIntro, Route128_Text_RubenDefeat
	msgbox Route128_Text_RubenPostBattle, MSGBOX_AUTOCLOSE
	end

Route128_EventScript_Wayne:: @ 815234E
	trainerbattle_single TRAINER_WAYNE, Route128_Text_WayneIntro, Route128_Text_WayneDefeat
	msgbox Route128_Text_WaynePostBattle, MSGBOX_AUTOCLOSE
	end
