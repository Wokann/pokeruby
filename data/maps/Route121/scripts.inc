Route121_MapScripts:: @ 8151AFE
	map_script MAP_SCRIPT_ON_TRANSITION, Route121_OnTransition
	.byte 0

Route121_OnTransition:: @ 8151B04
	call Common_EventScript_SetupEvilTeamGfxIds
	end

Route121_EventScript_Woman:: @ 8151B0A
	msgbox Route121_Text_AheadLoomsMtPyre, MSGBOX_NPC
	end

Route121_EventScript_MtPyrePierSign:: @ 8151B13
	msgbox Route121_Text_MtPyrePierSign, MSGBOX_SIGN
	end

Route121_EventScript_SafariZoneSign:: @ 8151B1C
	msgbox Route121_Text_SafariZoneSign, MSGBOX_SIGN
	end

Route121_EventScript_EvilTeamGruntsMoveOut:: @ 8151B25
	lockall
	msgbox Route121_Text_OkayMoveOutToMtPyre, MSGBOX_DEFAULT
	closemessage
	applymovement 12, Route121_Movement_Grunt1Exit
	applymovement 13, Route121_Movement_Grunt2Exit
	applymovement 14, Route121_Movement_Grunt3Exit
	waitmovement 0
	removeobject 12
	removeobject 13
	removeobject 14
	setvar VAR_ROUTE121_STATE, 1
	releaseall
	end

Route121_Movement_Grunt1Exit:: @ 8151B57
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route121_Movement_Grunt2Exit:: @ 8151B60
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route121_Movement_Grunt3Exit:: @ 8151B69
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route121_EventScript_Vanessa:: @ 8151B72
	trainerbattle_single TRAINER_VANESSA, Route121_Text_VanessaIntro, Route121_Text_VanessaDefeat
	msgbox Route121_Text_VanessaPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Walter:: @ 8151B89
	trainerbattle_single TRAINER_WALTER_1, Route121_Text_WalterIntro, Route121_Text_WalterDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route121_EventScript_RematchWalter
	msgbox Route121_Text_WalterPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_RematchWalter:: @ 8151BB0
	trainerbattle_rematch TRAINER_WALTER_1, Route121_Text_WalterRematchIntro, Route121_Text_WalterRematchDefeat
	msgbox Route121_Text_WalterPostRematch, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Tammy:: @ 8151BC7
	trainerbattle_single TRAINER_TAMMY, Route121_Text_TammyIntro, Route121_Text_TammyDefeat
	msgbox Route121_Text_TammyPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Kate:: @ 8151BDE
	trainerbattle_double TRAINER_KATE_AND_JOY, Route121_Text_KateIntro, Route121_Text_KateDefeat, Route121_Text_KateNotEnoughMons
	msgbox Route121_Text_KatePostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Joy:: @ 8151BF9
	trainerbattle_double TRAINER_KATE_AND_JOY, Route121_Text_JoyIntro, Route121_Text_JoyDefeat, Route121_Text_JoyNotEnoughMons
	msgbox Route121_Text_JoyPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Jessica:: @ 8151C14
	trainerbattle_single TRAINER_JESSICA_1, Route121_Text_JessicaIntro, Route121_Text_JessicaDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route121_EventScript_RematchJessica
	msgbox Route121_Text_JessicaPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_RematchJessica:: @ 8151C3B
	trainerbattle_rematch TRAINER_JESSICA_1, Route121_Text_JessicaRematchIntro, Route121_Text_JessicaRematchDefeat
	msgbox Route121_Text_JessicaPostRematch, MSGBOX_AUTOCLOSE
	end
