Route134_MapScripts:: @ 81525B5
	map_script MAP_SCRIPT_ON_RESUME, Route134_OnResume
	.byte 0

Route134_OnResume:: @ 81525BB
	setdivewarp MAP_UNDERWATER_ROUTE134, 255, 8, 6
	end

Route134_EventScript_Jack:: @ 81525C4
	trainerbattle_single TRAINER_JACK, Route134_Text_JackIntro, Route134_Text_JackDefeat
	msgbox Route134_Text_JackPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Laurel:: @ 81525DB
	trainerbattle_single TRAINER_LAUREL, Route134_Text_LaurelIntro, Route134_Text_LaurelDefeat
	msgbox Route134_Text_LaurelPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Alex:: @ 81525F2
	trainerbattle_single TRAINER_ALEX, Route134_Text_AlexIntro, Route134_Text_AlexDefeat
	msgbox Route134_Text_AlexPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Aaron:: @ 8152609
	trainerbattle_single TRAINER_AARON, Route134_Text_AaronIntro, Route134_Text_AaronDefeat
	msgbox Route134_Text_AaronPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Hitoshi:: @ 8152620
	trainerbattle_single TRAINER_HITOSHI, Route134_Text_HitoshiIntro, Route134_Text_HitoshiDefeat
	msgbox Route134_Text_HitoshiPostBattle, MSGBOX_AUTOCLOSE
	end
