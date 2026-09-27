MtPyre_4F_MapScripts:: @ 815D417
	.byte 0

MtPyre_4F_EventScript_Atsushi:: @ 815D418
	trainerbattle_single TRAINER_ATSUSHI, MtPyre_4F_Text_AtsushiIntro, MtPyre_4F_Text_AtsushiDefeat
	msgbox MtPyre_4F_Text_AtsushiPostBattle, MSGBOX_AUTOCLOSE
	end
