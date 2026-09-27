MtPyre_5F_MapScripts:: @ 815D42F
	.byte 0

MtPyre_5F_EventScript_Tasha:: @ 815D430
	trainerbattle_single TRAINER_TASHA, MtPyre_5F_Text_TashaIntro, MtPyre_5F_Text_TashaDefeat
	msgbox MtPyre_5F_Text_TashaPostBattle, MSGBOX_AUTOCLOSE
	end
