Route127_MapScripts:: @ 815206A
	map_script MAP_SCRIPT_ON_TRANSITION, Route127_OnTransition
	.byte 0

Route127_OnTransition:: @ 8152070
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetLegendaryWeather
	end

Route127_EventScript_15207A:: @ 815207A
	trainerbattle_single TRAINER_CALEB, Route127_Text_1C17A8, Route127_Text_1C17DD
	msgbox Route127_Text_1C17EC, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_152091:: @ 8152091
	trainerbattle_single TRAINER_CONNOR, Route127_Text_1C1828, Route127_Text_1C1860
	msgbox Route127_Text_1C1877, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Jonah:: @ 81520A8
	trainerbattle_single TRAINER_JONAH, Route127_Text_JonahIntro, Route127_Text_JonahDefeat
	msgbox Route127_Text_JonahPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Henry:: @ 81520BF
	trainerbattle_single TRAINER_HENRY, Route127_Text_HenryIntro, Route127_Text_HenryDefeat
	msgbox Route127_Text_HenryPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Roger:: @ 81520D6
	trainerbattle_single TRAINER_ROGER, Route127_Text_RogerIntro, Route127_Text_RogerDefeat
	msgbox Route127_Text_RogerPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Koji:: @ 81520ED
	trainerbattle_single TRAINER_KOJI_1, Route127_Text_KojiIntro, Route127_Text_KojiDefeat
	msgbox Route127_Text_KojiPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_152104:: @ 8152104
	trainerbattle_single TRAINER_BYRON, Route127_Text_1C1B92, Route127_Text_1C1BD9
	msgbox Route127_Text_1C1BEC, MSGBOX_AUTOCLOSE
	end
