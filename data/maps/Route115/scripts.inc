Route115_MapScripts:: @ 815092D
	.byte 0

Route115_EventScript_Woman:: @ 815092E
	msgbox Route115_Text_NeverKnowWhenCavePokemonWillAppear, MSGBOX_NPC
	end

Route115_EventScript_RouteSignRustboro:: @ 8150937
	msgbox Route115_Text_RouteSignRustboro, MSGBOX_SIGN
	end

Route115_EventScript_MeteorFallsSign:: @ 8150940
	msgbox Route115_Text_MeteorFallsSign, MSGBOX_SIGN
	end

Route115_EventScript_Timothy:: @ 8150949
	trainerbattle_single TRAINER_TIMOTHY_1, Route115_Text_TimothyIntro, Route115_Text_TimothyDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route115_EventScript_RematchTimothy
	msgbox Route115_Text_TimothyPostBattle, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_RematchTimothy:: @ 8150970
	trainerbattle_rematch TRAINER_TIMOTHY_1, Route115_Text_TimothyRematchIntro, Route115_Text_TimothyRematchDefeat
	msgbox Route115_Text_TimothyPostRematch, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_Koichi:: @ 8150987
	trainerbattle_single TRAINER_KOICHI, Route115_Text_KoichiIntro, Route115_Text_KoichiDefeat
	msgbox Route115_Text_KoichiPostBattle, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_Nob:: @ 815099E
	trainerbattle_single TRAINER_NOB_1, Route115_Text_NobIntro, Route115_Text_NobDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route115_EventScript_RematchNob
	msgbox Route115_Text_NobPostBattle, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_RematchNob:: @ 81509C5
	trainerbattle_rematch TRAINER_NOB_1, Route115_Text_NobRematchIntro, Route115_Text_NobRematchDefeat
	msgbox Route115_Text_NobPostRematch, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_Cyndy:: @ 81509DC
	trainerbattle_single TRAINER_CYNDY_1, Route115_Text_CyndyIntro, Route115_Text_CyndyDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route115_EventScript_RematchCyndy
	msgbox Route115_Text_CyndyPostBattle, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_RematchCyndy:: @ 8150A03
	trainerbattle_rematch TRAINER_CYNDY_1, Route115_Text_CyndyRematchIntro, Route115_Text_CyndyRematchDefeat
	msgbox Route115_Text_CyndyPostRematch, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_Hector:: @ 8150A1A
.ifdef SAPPHIRE
	trainerbattle_single TRAINER_ROUTE115_HECTOR, Route115_Text_HectorSapphireIntro, Route115_Text_HectorSapphireDefeat
	msgbox Route115_Text_HectorSapphirePostBattle, MSGBOX_AUTOCLOSE
.else
	trainerbattle_single TRAINER_ROUTE115_HECTOR, Route115_Text_HectorIntro, Route115_Text_HectorDefeat
	msgbox Route115_Text_HectorPostBattle, MSGBOX_AUTOCLOSE
.endc
	end
