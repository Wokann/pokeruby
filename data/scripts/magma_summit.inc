MtPyre_Summit_EventScript_Grunt1:: @ 81B2DA9
	.ifdef SAPPHIRE
	trainerbattle_single TRAINER_MT_PYRE_SUMMIT_GRUNT_1, MtPyre_Summit_Text_Grunt1Intro, MtPyre_Summit_Text_Grunt1Defeat
	msgbox MtPyre_Summit_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	.else
	trainerbattle_single TRAINER_MT_PYRE_SUMMIT_GRUNT_1, MtPyre_Summit_Text_1B5CFD, MtPyre_Summit_Text_1B5D30
	msgbox MtPyre_Summit_Text_1B5D6D, MSGBOX_AUTOCLOSE
	.endif
	end

MtPyre_Summit_EventScript_Grunt2:: @ 81B2DC0
	.ifdef SAPPHIRE
	trainerbattle_single TRAINER_MT_PYRE_SUMMIT_GRUNT_2, MtPyre_Summit_Text_Grunt2Intro, MtPyre_Summit_Text_Grunt2Defeat
	msgbox MtPyre_Summit_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	.else
	trainerbattle_single TRAINER_MT_PYRE_SUMMIT_GRUNT_2, MtPyre_Summit_Text_1B5DA2, MtPyre_Summit_Text_1B5DDB
	msgbox MtPyre_Summit_Text_1B5DF1, MSGBOX_AUTOCLOSE
	.endif
	end

MtPyre_Summit_EventScript_Grunt3:: @ 81B2DD7
	.ifdef SAPPHIRE
	trainerbattle_single TRAINER_MT_PYRE_SUMMIT_GRUNT_3, MtPyre_Summit_Text_Grunt3Intro, MtPyre_Summit_Text_Grunt3Defeat
	msgbox MtPyre_Summit_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	.else
	trainerbattle_single TRAINER_MT_PYRE_SUMMIT_GRUNT_3, MtPyre_Summit_Text_1B5E27, MtPyre_Summit_Text_1B5E60
	msgbox MtPyre_Summit_Text_1B5E84, MSGBOX_AUTOCLOSE
	.endif
	end
