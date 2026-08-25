Route130_MapScripts:: @ 81523C2
	map_script MAP_SCRIPT_ON_TRANSITION, Route130_OnTransition
	.byte 0

Route130_OnTransition:: @ 81523C8
	specialvar VAR_RESULT, IsMirageIslandPresent
	compare VAR_RESULT, TRUE
	goto_if_eq Route130_EventScript_SetMirageIslandLayout
	setflag FLAG_TEMP_HIDE_MIRAGE_ISLAND_BERRY_TREE
	setflag FLAG_TEMP_12
	setflag FLAG_TEMP_13
	setflag FLAG_TEMP_14
	setflag FLAG_TEMP_15
	setflag FLAG_TEMP_16
	setflag FLAG_TEMP_17
	setflag FLAG_TEMP_18
	setflag FLAG_TEMP_19
	setflag FLAG_TEMP_1A
	setflag FLAG_TEMP_1B
	setflag FLAG_TEMP_1C
	setflag FLAG_TEMP_1D
	setflag FLAG_TEMP_1E
	setflag FLAG_TEMP_1F
	setflag FLAG_TEMP_20
	setmaplayoutindex LAYOUT_ROUTE130
	end

Route130_EventScript_SetMirageIslandLayout:: @ 815240C
	setmaplayoutindex LAYOUT_ROUTE130_MIRAGE_ISLAND
	end

Route130_EventScript_Rodney:: @ 8152410
	trainerbattle_single TRAINER_RODNEY, Route130_Text_RodneyIntro, Route130_Text_RodneyDefeat
	msgbox Route130_Text_RodneyPostBattle, MSGBOX_AUTOCLOSE
	end

Route130_EventScript_Katie:: @ 8152427
	trainerbattle_single TRAINER_KATIE, Route130_Text_KatieIntro, Route130_Text_KatieDefeat
	msgbox Route130_Text_KatiePostBattle, MSGBOX_AUTOCLOSE
	end
