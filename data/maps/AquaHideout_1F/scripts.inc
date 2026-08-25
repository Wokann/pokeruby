AquaHideout_1F_MapScripts:: @ 815D7C2
	map_script MAP_SCRIPT_ON_TRANSITION, AquaHideout_1F_OnTransition
	.byte 0

AquaHideout_1F_OnTransition:: @ 815D7C8
	call Common_EventScript_SetupEvilTeamGfxIds
	end

AquaHideout_1F_EventScript_EvilTeamEntranceGrunt1:: @ 815D7CE
MagmaHideout_1F_EventScript_EvilTeamEntranceGrunt1:: @ 815D7CE
	lock
	faceplayer
	goto_if_set FLAG_MT_PYRE_ORB_STOLEN, Hideout_1F_EventScript_EvilTeamEntranceGrunt1AfterOrbStolen
	msgbox Hideout_1F_Text_OurBossIsSnatchingSomething, MSGBOX_DEFAULT
	release
	end

Hideout_1F_EventScript_EvilTeamEntranceGrunt1AfterOrbStolen:: @ 815D7E3
	msgbox Hideout_1F_Text_BossWentToJackASubmarine, MSGBOX_DEFAULT
	release
	end

AquaHideout_1F_EventScript_EvilTeamEntranceGrunt2:: @ 815D7ED
MagmaHideout_1F_EventScript_EvilTeamEntranceGrunt2:: @ 815D7ED
	lock
	faceplayer
	goto_if_set FLAG_MT_PYRE_ORB_STOLEN, Hideout_1F_EventScript_EvilTeamEntranceGrunt2AfterOrbStolen
	msgbox Hideout_1F_Text_BossIsOnRoute122, MSGBOX_DEFAULT
	release
	end

Hideout_1F_EventScript_EvilTeamEntranceGrunt2AfterOrbStolen:: @ 815D802
	msgbox Hideout_1F_Text_BossIsInSlateportCity, MSGBOX_DEFAULT
	release
	end

AquaHideout_1F_EventScript_EvilTeamGrunt1:: @ 815D80C
MagmaHideout_1F_EventScript_EvilTeamGrunt1:: @ 815D80C
	trainerbattle_single TRAINER_HIDEOUT_1F_GRUNT, Hideout_1F_Text_EvilTeamGrunt1Intro, Hideout_1F_Text_EvilTeamGrunt1Defeat, Hideout_1F_EventScript_EvilTeamGrunt1Defeated
	msgbox Hideout_1F_Text_EvilTeamGrunt1PostBattle, MSGBOX_AUTOCLOSE
	end

Hideout_1F_EventScript_EvilTeamGrunt1Defeated:: @ 815D827
	msgbox Hideout_1F_Text_EvilTeamGrunt1PostBattle, MSGBOX_DEFAULT
	release
	end
