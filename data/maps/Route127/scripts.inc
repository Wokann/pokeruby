Route127_MapScripts:: @ 815206A
	map_script MAP_SCRIPT_ON_TRANSITION, Route127_OnTransition
	.byte 0

Route127_OnTransition:: @ 8152070
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetLegendaryWeather
	end

Route127_EventScript_Caleb:: @ 815207A
	trainerbattle_single TRAINER_CALEB, Route127_Text_CalebIntro, Route127_Text_CalebDefeat
	msgbox Route127_Text_CalebPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Connor:: @ 8152091
	trainerbattle_single TRAINER_CONNOR, Route127_Text_ConnorIntro, Route127_Text_ConnorDefeat
	msgbox Route127_Text_ConnorPostBattle, MSGBOX_AUTOCLOSE
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
	trainerbattle_single TRAINER_KOJI, Route127_Text_KojiIntro, Route127_Text_KojiDefeat
	msgbox Route127_Text_KojiPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Byron:: @ 8152104
	trainerbattle_single TRAINER_BYRON, Route127_Text_ByronIntro, Route127_Text_ByronDefeat
	msgbox Route127_Text_ByronPostBattle, MSGBOX_AUTOCLOSE
	end
