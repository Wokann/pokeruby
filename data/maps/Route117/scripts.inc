Route117_MapScripts:: @ 8150D3A
	map_script MAP_SCRIPT_ON_TRANSITION, Route117_OnTransition
	.byte 0

Route117_OnTransition:: @ 8150D40
	call Route117_EventScript_TryMoveDayCareMan
	end

Route117_EventScript_TryMoveDayCareMan:: @ 8150D46
	goto_if_unset FLAG_PENDING_DAYCARE_EGG, Route117_EventScript_StopMoveDayCareMan
	setobjectxyperm LOCALID_DAYCARE_MAN, 47, 6

Route117_EventScript_StopMoveDayCareMan:: @ 8150D56
	return

Route117_EventScript_Woman:: @ 8150D57
	msgbox Route117_Text_ArentTheseFlowersPretty, MSGBOX_NPC
	end

Route117_EventScript_LittleBoy:: @ 8150D60
	msgbox Route117_Text_AirIsTastyHere, MSGBOX_NPC
	end

Route117_EventScript_Girl:: @ 8150D69
	msgbox Route117_Text_DayCarePokemonHadNewMove, MSGBOX_NPC
	end

Route117_EventScript_RouteSignVerdanturf:: @ 8150D72
	msgbox Route117_Text_RouteSignVerdanturf, MSGBOX_SIGN
	end

Route117_EventScript_RouteSignMauville:: @ 8150D7B
	msgbox Route117_Text_RouteSignMauville, MSGBOX_SIGN
	end

Route117_EventScript_DayCareSign:: @ 8150D84
	msgbox Route117_Text_DayCareSign, MSGBOX_SIGN
	end

Route117_EventScript_Isaac:: @ 8150D8D
	trainerbattle_single TRAINER_ISAAC_1, Route117_Text_IsaacIntro, Route117_Text_IsaacDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route117_EventScript_RematchIsaac
	msgbox Route117_Text_IsaacPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_RematchIsaac:: @ 8150DB4
	trainerbattle_rematch TRAINER_ISAAC_1, Route117_Text_IsaacRematchIntro, Route117_Text_IsaacRematchDefeat
	msgbox Route117_Text_IsaacPostRematch, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Lydia:: @ 8150DCB
	trainerbattle_single TRAINER_LYDIA_1, Route117_Text_LydiaIntro, Route117_Text_LydiaDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route117_EventScript_RematchLydia
	msgbox Route117_Text_LydiaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_RematchLydia:: @ 8150DF2
	trainerbattle_rematch TRAINER_LYDIA_1, Route117_Text_LydiaRematchIntro, Route117_Text_LydiaRematchDefeat
	msgbox Route117_Text_LydiaPostRematch, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Dylan:: @ 8150E09
	trainerbattle_single TRAINER_DYLAN_1, Route117_Text_DylanIntro, Route117_Text_DylanDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route117_EventScript_RematchDylan
	msgbox Route117_Text_DylanPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_RematchDylan:: @ 8150E30
	trainerbattle_rematch TRAINER_DYLAN_1, Route117_Text_DylanRematchIntro, Route117_Text_DylanRematchDefeat
	msgbox Route117_Text_DylanPostRematch, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Maria:: @ 8150E47
	trainerbattle_single TRAINER_MARIA_1, Route117_Text_MariaIntro, Route117_Text_MariaDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route117_EventScript_RematchMaria
	msgbox Route117_Text_MariaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_RematchMaria:: @ 8150E6E
	trainerbattle_rematch TRAINER_MARIA_1, Route117_Text_MariaRematchIntro, Route117_Text_MariaRematchDefeat
	msgbox Route117_Text_MariaPostRematch, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Derek:: @ 8150E85
	trainerbattle_single TRAINER_DEREK, Route117_Text_DerekIntro, Route117_Text_DerekDefeat
	msgbox Route117_Text_DerekPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Anna:: @ 8150E9C
	trainerbattle_double TRAINER_ANNA_AND_MEG_1, Route117_Text_AnnaIntro, Route117_Text_AnnaDefeat, Route117_Text_AnnaNotEnoughMons
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route117_EventScript_RematchAnna
	msgbox Route117_Text_AnnaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_RematchAnna:: @ 8150EC7
	trainerbattle_rematch_double TRAINER_ANNA_AND_MEG_1, Route117_Text_AnnaRematchIntro, Route117_Text_AnnaRematchDefeat, Route117_Text_AnnaRematchNotEnoughMons
	msgbox Route117_Text_AnnaPostRematch, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Meg:: @ 8150EE2
	trainerbattle_double TRAINER_ANNA_AND_MEG_1, Route117_Text_MegIntro, Route117_Text_MegDefeat, Route117_Text_MegNotEnoughMons
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route117_EventScript_RematchMeg
	msgbox Route117_Text_MegPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_RematchMeg:: @ 8150F0D
	trainerbattle_rematch_double TRAINER_ANNA_AND_MEG_1, Route117_Text_MegRematchIntro, Route117_Text_MegRematchDefeat, Route117_Text_MegRematchNotEnoughMons
	msgbox Route117_Text_MegPostRematch, MSGBOX_AUTOCLOSE
	end
