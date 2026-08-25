Route124_MapScripts:: @ 8151DDF
	map_script MAP_SCRIPT_ON_TRANSITION, Route124_OnTransition
	.byte 0

Route124_OnTransition:: @ 8151DE5
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetLegendaryWeather
	end

Route124_EventScript_HuntersHouseSign:: @ 8151DEF
	msgbox Route124_Text_HuntersHouse, MSGBOX_SIGN
	end

Route124_EventScript_Spencer:: @ 8151DF8
	trainerbattle_single TRAINER_SPENCER, Route124_Text_SpencerIntro, Route124_Text_SpencerDefeat
	msgbox Route124_Text_SpencerPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Roland:: @ 8151E0F
	trainerbattle_single TRAINER_ROLAND, Route124_Text_RolandIntro, Route124_Text_RolandDefeat
	msgbox Route124_Text_RolandPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Jenny:: @ 8151E26
	trainerbattle_single TRAINER_JENNY_1, Route124_Text_JennyIntro, Route124_Text_JennyDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route124_EventScript_RematchJenny
	msgbox Route124_Text_JennyPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_RematchJenny:: @ 8151E4D
	trainerbattle_rematch TRAINER_JENNY_1, Route124_Text_JennyRematchIntro, Route124_Text_JennyRematchDefeat
	msgbox Route124_Text_JennyPostRematch, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Grace:: @ 8151E64
	trainerbattle_single TRAINER_GRACE, Route124_Text_GraceIntro, Route124_Text_GraceDefeat
	msgbox Route124_Text_GracePostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Chad:: @ 8151E7B
	trainerbattle_single TRAINER_CHAD, Route124_Text_ChadIntro, Route124_Text_ChadDefeat
	msgbox Route124_Text_ChadPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Rita:: @ 8151E92
	trainerbattle_double TRAINER_RITA_AND_SAM_1, Route124_Text_RitaIntro, Route124_Text_RitaDefeat, Route124_Text_RitaNotEnoughMons
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route124_EventScript_RematchRita
	msgbox Route124_Text_RitaPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_RematchRita:: @ 8151EBD
	trainerbattle_rematch_double TRAINER_RITA_AND_SAM_1, Route124_Text_RitaRematchIntro, Route124_Text_RitaRematchDefeat, Route124_Text_RitaRematchNotEnoughMons
	msgbox Route124_Text_RitaPostRematch, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Sam:: @ 8151ED8
	trainerbattle_double TRAINER_RITA_AND_SAM_1, Route124_Text_SamIntro, Route124_Text_SamDefeat, Route124_Text_SamNotEnoughMons
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route124_EventScript_RematchSam
	msgbox Route124_Text_SamPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_RematchSam:: @ 8151F03
	trainerbattle_rematch_double TRAINER_RITA_AND_SAM_1, Route124_Text_SamRematchIntro, Route124_Text_SamRematchDefeat, Route124_Text_SamRematchNotEnoughMons
	msgbox Route124_Text_SamPostRematch, MSGBOX_AUTOCLOSE
	end
