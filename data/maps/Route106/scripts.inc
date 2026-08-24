Route106_MapScripts:: @ 814F32E
	.byte 0

Route106_EventScript_TrainerTipsSign:: @ 814F32F
	msgbox Route106_Text_TrainerTips, MSGBOX_SIGN
	end

Route106_EventScript_Douglas:: @ 814F338
	trainerbattle_single TRAINER_DOUGLAS, Route106_Text_DouglasIntro, Route106_Text_DouglasDefeated
	msgbox Route106_Text_DouglasPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_EventScript_Nicole:: @ 814F34F
	trainerbattle_single TRAINER_NICOLE, Route106_Text_NicoleIntro, Route106_Text_NicoleDefeated
	msgbox Route106_Text_NicolePostBattle, MSGBOX_AUTOCLOSE
	end

Route106_EventScript_Elliot:: @ 814F366
	trainerbattle_single TRAINER_ELLIOT_1, Route106_Text_ElliotIntro, Route106_Text_ElliotDefeated
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route106_EventScript_ElliotRematch
	msgbox Route106_Text_ElliotPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_EventScript_ElliotRematch:: @ 814F38D
	trainerbattle_rematch TRAINER_ELLIOT_1, Route106_Text_ElliotRematchIntro, Route106_Text_ElliotRematchDefeated
	msgbox Route106_Text_ElliotRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_EventScript_Ned:: @ 814F3A4
	trainerbattle_single TRAINER_NED, Route106_Text_NedIntro, Route106_Text_NedDefeated
	msgbox Route106_Text_NedPostBattle, MSGBOX_AUTOCLOSE
	end
