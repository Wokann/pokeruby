Route132_MapScripts:: @ 81524E4
	.byte 0

Route132_EventScript_Gilbert:: @ 81524E5
	trainerbattle_single TRAINER_GILBERT, Route132_Text_GilbertIntro, Route132_Text_GilbertDefeat
	msgbox Route132_Text_GilbertPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Dana:: @ 81524FC
	trainerbattle_single TRAINER_DANA, Route132_Text_DanaIntro, Route132_Text_DanaDefeat
	msgbox Route132_Text_DanaPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Ronald:: @ 8152513
	trainerbattle_single TRAINER_RONALD, Route132_Text_RonaldIntro, Route132_Text_RonaldDefeat
	msgbox Route132_Text_RonaldPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Kiyo:: @ 815252A
	trainerbattle_single TRAINER_KIYO, Route132_Text_KiyoIntro, Route132_Text_KiyoDefeat
	msgbox Route132_Text_KiyoPostBattle, MSGBOX_AUTOCLOSE
	end
