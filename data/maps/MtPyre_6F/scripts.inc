MtPyre_6F_MapScripts:: @ 815D447
	.byte 0

MtPyre_6F_EventScript_Valerie:: @ 815D448
	trainerbattle_single TRAINER_VALERIE_1, MtPyre_6F_Text_ValerieIntro, MtPyre_6F_Text_ValerieDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq MtPyre_6F_EventScript_ValerieRematch
	msgbox MtPyre_6F_Text_ValeriePostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_EventScript_ValerieRematch:: @ 815D46F
	trainerbattle_rematch TRAINER_VALERIE_1, MtPyre_6F_Text_ValerieRematchIntro, MtPyre_6F_Text_ValerieRematchDefeat
	msgbox MtPyre_6F_Text_ValeriePostRematch, MSGBOX_AUTOCLOSE
	end
