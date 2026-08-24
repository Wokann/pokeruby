Route114_MapScripts:: @ 8150732
	.byte 0

Route114_EventScript_Man:: @ 8150733
	lock
	faceplayer
	dotimebasedevents
	goto_if_set FLAG_DAILY_RECEIVED_BERRY_ROUTE114, Route114_EventScript_ReceivedBerry
	msgbox Route114_Text_LoveUsingBerryCrushShareBerry, MSGBOX_DEFAULT
	random 5
	addvar VAR_RESULT, 15
	addvar VAR_RESULT, 133
	giveitem VAR_RESULT
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_DAILY_RECEIVED_BERRY_ROUTE114
	msgbox Route114_Text_TryBerryCrushWithFriends, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_ReceivedBerry:: @ 8150778
	msgbox Route114_Text_FunToThinkAboutBerries, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_RoarGentleman:: @ 8150782
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM05, Route114_EventScript_ReceivedRoar
	msgbox Route114_Text_AllMyMonDoesIsRoarTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_TM05_ROAR
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM05
	msgbox Route114_Text_ExplainRoar, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_ReceivedRoar:: @ 81507B9
	msgbox Route114_Text_ExplainRoar, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_Poochyena:: @ 81507C3
	lock
	faceplayer
	waitse
	playmoncry SPECIES_POOCHYENA, 2
	msgbox Route114_Text_Poochyena, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route114_EventScript_MeteorFallsSign:: @ 81507D6
	msgbox Route114_Text_MeteorFallsSign, MSGBOX_SIGN
	end

Route114_EventScript_FossilManiacsHouseSign:: @ 81507DF
	msgbox Route114_Text_FossilManiacsHouseSign, MSGBOX_SIGN
	end

Route114_EventScript_LanettesHouseSign:: @ 81507E8
	msgbox Route114_Text_LanettesHouse, MSGBOX_SIGN
	end

Route114_EventScript_Lenny:: @ 81507F1
	trainerbattle_single TRAINER_LENNY, Route114_Text_LennyIntro, Route114_Text_LennyDefeat
	msgbox Route114_Text_LennyPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Lucas:: @ 8150808
	trainerbattle_single TRAINER_LUCAS_1, Route114_Text_LucasIntro, Route114_Text_LucasDefeat
	msgbox Route114_Text_LucasPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Shane:: @ 815081F
	trainerbattle_single TRAINER_SHANE, Route114_Text_ShaneIntro, Route114_Text_ShaneDefeat
	msgbox Route114_Text_ShanePostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Nancy:: @ 8150836
	trainerbattle_single TRAINER_NANCY, Route114_Text_NancyIntro, Route114_Text_NancyDefeat
	msgbox Route114_Text_NancyPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Steve:: @ 815084D
	trainerbattle_single TRAINER_STEVE_1, Route114_Text_SteveIntro, Route114_Text_SteveDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route114_EventScript_RematchSteve
	msgbox Route114_Text_StevePostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_RematchSteve:: @ 8150874
	trainerbattle_rematch TRAINER_STEVE_1, Route114_Text_SteveRematchIntro, Route114_Text_SteveRematchDefeat
	msgbox Route114_Text_StevePostRematch, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Bernie:: @ 815088B
	trainerbattle_single TRAINER_BERNIE_1, Route114_Text_BernieIntro, Route114_Text_BernieDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route114_EventScript_RematchBernie
	msgbox Route114_Text_BerniePostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_RematchBernie:: @ 81508B2
	trainerbattle_rematch TRAINER_BERNIE_1, Route114_Text_BernieRematchIntro, Route114_Text_BernieRematchDefeat
	msgbox Route114_Text_BerniePostRematch, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Claude:: @ 81508C9
	trainerbattle_single TRAINER_CLAUDE, Route114_Text_ClaudeIntro, Route114_Text_ClaudeDefeat
	msgbox Route114_Text_ClaudePostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Nolan:: @ 81508E0
	trainerbattle_single TRAINER_NOLAN, Route114_Text_NolanIntro, Route114_Text_NolanDefeat
	msgbox Route114_Text_NolanPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Tyra:: @ 81508F7
	trainerbattle_double TRAINER_TYRA_AND_IVY, Route114_Text_TyraIntro, Route114_Text_TyraDefeat, Route114_Text_TyraNotEnoughMons
	msgbox Route114_Text_TyraPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Ivy:: @ 8150912
	trainerbattle_double TRAINER_TYRA_AND_IVY, Route114_Text_IvyIntro, Route114_Text_IvyDefeat, Route114_Text_IvyNotEnoughMons
	msgbox Route114_Text_IvyPostBattle, MSGBOX_AUTOCLOSE
	end
