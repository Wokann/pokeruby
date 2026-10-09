AquaHideout_B1F_MapScripts:: @ 815D831
	map_script MAP_SCRIPT_ON_RESUME, AquaHideout_B1F_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, AquaHideout_B1F_OnTransition
	.byte 0

AquaHideout_B1F_OnResume:: @ 815D83C
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, AquaHideout_B1F_EventScript_TryRemoveElectrode
	end

AquaHideout_B1F_EventScript_TryRemoveElectrode:: @ 815D846
	removeobject VAR_LAST_TALKED
	return

AquaHideout_B1F_OnTransition:: @ 815D84A
	call Common_EventScript_SetupEvilTeamGfxIds
	end

AquaHideout_B1F_EventScript_Grunt2:: @ 815D850
MagmaHideout_B1F_EventScript_15D850:: @ 815D850
	trainerbattle_single TRAINER_HIDEOUT_B1F_GRUNT_1, AquaHideout_B1F_Text_Grunt2Intro, AquaHideout_B1F_Text_Grunt2Defeat, AquaHideout_B1F_EventScript_Grunt2Defeated
	msgbox AquaHideout_B1F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

AquaHideout_B1F_EventScript_Grunt2Defeated:: @ 815D86B
	msgbox AquaHideout_B1F_Text_Grunt2PostBattle, MSGBOX_DEFAULT
	release
	end

AquaHideout_B1F_EventScript_Grunt3:: @ 815D875
MagmaHideout_B1F_EventScript_15D875:: @ 815D875
	trainerbattle_single TRAINER_HIDEOUT_B1F_GRUNT_2, AquaHideout_B1F_Text_Grunt3Intro, AquaHideout_B1F_Text_Grunt3Defeat, AquaHideout_B1F_EventScript_Grunt3Defeated
	msgbox AquaHideout_B1F_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

AquaHideout_B1F_EventScript_Grunt3Defeated:: @ 815D890
	msgbox AquaHideout_B1F_Text_Grunt3PostBattle, MSGBOX_DEFAULT
	release
	end

AquaHideout_B1F_EventScript_Grunt5:: @ 815D89A
MagmaHideout_B1F_EventScript_15D89A:: @ 815D89A
	trainerbattle_single TRAINER_HIDEOUT_B1F_GRUNT_3, AquaHideout_B1F_Text_Grunt5Intro, AquaHideout_B1F_Text_Grunt5Defeat
	msgbox AquaHideout_B1F_Text_Grunt5PostBattle, MSGBOX_AUTOCLOSE
	end
