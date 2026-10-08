Route107_MapScripts:: @ 814F3BB
	.byte 0

Route107_EventScript_Darrin:: @ 814F3BC
	trainerbattle_single TRAINER_DARRIN, Route107_Text_DarrinIntro, Route107_Text_DarrinDefeated
	msgbox Route107_Text_DarrinPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Tony:: @ 814F3D3
	trainerbattle_single TRAINER_TONY_1, Route107_Text_TonyIntro, Route107_Text_TonyDefeated
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route107_EventScript_TonyRematch
	msgbox Route107_Text_TonyPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_TonyRematch:: @ 814F3FA
	trainerbattle_rematch TRAINER_TONY_1, Route107_Text_TonyRematchIntro, Route107_Text_TonyRematchDefeated
	msgbox Route107_Text_TonyRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Denise:: @ 814F411
	trainerbattle_single TRAINER_DENISE, Route107_Text_DeniseIntro, Route107_Text_DeniseDefeated
	msgbox Route107_Text_DenisePostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Beth:: @ 814F428
	trainerbattle_single TRAINER_BETH, Route107_Text_BethIntro, Route107_Text_BethDefeated
	msgbox Route107_Text_BethPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Lisa:: @ 814F43F
	trainerbattle_double TRAINER_LISA_AND_RAY, Route107_Text_LisaIntro, Route107_Text_LisaDefeated, Route107_Text_LisaNotEnoughPokemon
	msgbox Route107_Text_LisaPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Ray:: @ 814F45A
	trainerbattle_double TRAINER_LISA_AND_RAY, Route107_Text_RayIntro, Route107_Text_RayDefeated, Route107_Text_RayNotEnoughPokemon
	msgbox Route107_Text_RayPostBattle, MSGBOX_AUTOCLOSE
	end
