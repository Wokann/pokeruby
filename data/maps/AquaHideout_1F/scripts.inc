AquaHideout_1F_MapScripts:: @ 815D7C2
	map_script MAP_SCRIPT_ON_TRANSITION, AquaHideout_1F_MapScript1_OnTransition
	.byte 0

AquaHideout_1F_MapScript1_OnTransition:: @ 815D7C8
	call Common_EventScript_SetupEvilTeamGfxIds
	end

AquaHideout_1F_EventScript_HideoutEntranceGrunt1:: @ 815D7CE
MagmaHideout_1F_EventScript_HideoutEntranceGrunt1:: @ 815D7CE
	lock
	faceplayer
	goto_if_set FLAG_MT_PYRE_ORB_STOLEN, AquaHideout_1F_EventScript_Grunt1SubmarineHint
	msgbox AquaHideout_1F_Text_OurBossIsSnatchingSomething, MSGBOX_DEFAULT
	release
	end

AquaHideout_1F_EventScript_Grunt1SubmarineHint:: @ 815D7E3
	msgbox AquaHideout_1F_Text_BossWentToJackASubmarine, MSGBOX_DEFAULT
	release
	end

AquaHideout_1F_EventScript_HideoutEntranceGrunt2:: @ 815D7ED
MagmaHideout_1F_EventScript_HideoutEntranceGrunt2:: @ 815D7ED
	lock
	faceplayer
	goto_if_set FLAG_MT_PYRE_ORB_STOLEN, AquaHideout_1F_EventScript_Grunt2SlateportHint
	msgbox AquaHideout_1F_Text_BossIsOnRoute122, MSGBOX_DEFAULT
	release
	end

AquaHideout_1F_EventScript_Grunt2SlateportHint:: @ 815D802
	msgbox AquaHideout_1F_Text_BossIsInSlateportCity, MSGBOX_DEFAULT
	release
	end

AquaHideout_1F_EventScript_Grunt1:: @ 815D80C
MagmaHideout_1F_EventScript_Grunt1:: @ 815D80C
	trainerbattle_single TRAINER_HIDEOUT_1F_GRUNT, AquaHideout_1F_Text_Grunt1Intro, AquaHideout_1F_Text_Grunt1Defeat, AquaHideout_1F_EventScript_Grunt1Defeated
	msgbox AquaHideout_1F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

AquaHideout_1F_EventScript_Grunt1Defeated:: @ 815D827
	msgbox AquaHideout_1F_Text_Grunt1PostBattle, MSGBOX_DEFAULT
	release
	end
