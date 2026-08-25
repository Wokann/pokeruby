Route112_MapScripts:: @ 8150502
	map_script MAP_SCRIPT_ON_TRANSITION, Route112_OnTransition
	.byte 0

Route112_OnTransition:: @ 8150508
	call Common_EventScript_SetupEvilTeamGfxIds
	setvar VAR_JAGGED_PASS_ASH_WEATHER, 0
	end

Route112_EventScript_EvilTeamGrunt1:: @ 8150513
	msgbox Route112_Text_EvilTeamGrunt1WaitingForFallarborCrew, MSGBOX_NPC
	end

Route112_EventScript_EvilTeamGrunt2:: @ 815051C
	msgbox Route112_Text_EvilTeamGrunt2WantsCableCarRide, MSGBOX_NPC
	end

Route112_EventScript_MtChimneyCableCarSign:: @ 8150525
	msgbox Route112_Text_MtChimneyCableCarSign, MSGBOX_SIGN
	end

Route112_EventScript_MtChimneySign:: @ 815052E
	msgbox Route112_Text_MtChimneySign, MSGBOX_SIGN
	end

Route112_EventScript_RouteSignLavaridge:: @ 8150537
	msgbox Route112_Text_RouteSignLavaridge, MSGBOX_SIGN
	end

Route112_EventScript_Hiker:: @ 8150540
	msgbox Route112_Text_NotEasyToGetBackToLavaridge, MSGBOX_NPC
	end

Route112_EventScript_Brice:: @ 8150549
	trainerbattle_single TRAINER_BRICE, Route112_Text_BriceIntro, Route112_Text_BriceDefeat
	msgbox Route112_Text_BricePostBattle, MSGBOX_AUTOCLOSE
	end

Route112_EventScript_Trent:: @ 8150560
	trainerbattle_single TRAINER_TRENT_1, Route112_Text_TrentIntro, Route112_Text_TrentDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route112_EventScript_RematchTrent
	msgbox Route112_Text_TrentPostBattle, MSGBOX_AUTOCLOSE
	end

Route112_EventScript_RematchTrent:: @ 8150587
	trainerbattle_rematch TRAINER_TRENT_1, Route112_Text_TrentRematchIntro, Route112_Text_TrentRematchDefeat
	msgbox Route112_Text_TrentRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route112_EventScript_Larry:: @ 815059E
	trainerbattle_single TRAINER_LARRY, Route112_Text_LarryIntro, Route112_Text_LarryDefeat
	msgbox Route112_Text_LarryPostBattle, MSGBOX_AUTOCLOSE
	end

Route112_EventScript_Carol:: @ 81505B5
	trainerbattle_single TRAINER_CAROL, Route112_Text_CarolIntro, Route112_Text_CarolDefeat
	msgbox Route112_Text_CarolPostBattle, MSGBOX_AUTOCLOSE
	end
