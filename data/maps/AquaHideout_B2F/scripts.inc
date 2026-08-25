AquaHideout_B2F_MapScripts:: @ 815D8B1
	map_script MAP_SCRIPT_ON_TRANSITION, AquaHideout_B2F_OnTransition
	.byte 0

AquaHideout_B2F_OnTransition:: @ 815D8B7
	call Common_EventScript_SetupEvilTeamGfxIds
	end

AquaHideout_B2F_EventScript_EvilTeamAdminNoticePlayer:: @ 815D8BD
MagmaHideout_B2F_EventScript_EvilTeamAdminNoticePlayer:: @ 815D8BD
	lockall
	setvar VAR_0x8008, 1
	playse SE_PIN
	applymovement VAR_0x8008, Common_Movement_ExclamationMark
	waitmovement 0
	.ifdef SAPPHIRE
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	.else
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFastestUp
	.endif
	waitmovement 0
	setvar VAR_TEMP_1, 1
	releaseall
	end

AquaHideout_B2F_EventScript_EvilTeamAdmin:: @ 815D8E1
MagmaHideout_B2F_EventScript_EvilTeamAdmin:: @ 815D8E1
	trainerbattle_single TRAINER_HIDEOUT_B2F_GRUNT_1, Hideout_B2F_Text_EvilTeamAdminIntro, Hideout_B2F_Text_EvilTeamAdminDefeat, Hideout_B2F_EventScript_EvilTeamAdminEscapesWithSubmarine
	msgbox Hideout_B2F_Text_EvilTeamAdminPostBattle, MSGBOX_DEFAULT
	release
	end

Hideout_B2F_EventScript_EvilTeamAdminEscapesWithSubmarine:: @ 815D8FD
	setvar VAR_0x8008, 1
	.ifdef SAPPHIRE
	setvar VAR_0x8009, 4
	.else
	setvar VAR_0x8009, 3
	.endif
	.ifdef SAPPHIRE
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFastestLeft
	.else
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFastestRight
	.endif
	waitmovement 0
	delay 20
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	waitmovement 0
	msgbox Hideout_B2F_Text_BossPreparedSubmarine, MSGBOX_DEFAULT
	closemessage
	.ifdef SAPPHIRE
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFastestLeft
	.else
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFastestRight
	.endif
	.ifdef SAPPHIRE
	applymovement VAR_0x8009, Hideout_B2F_Movement_SubmarineDepartLeft
	.else
	applymovement VAR_0x8009, Hideout_B2F_Movement_SubmarineDepartRight
	.endif
	waitmovement 0
	removeobject VAR_0x8009
	delay 20
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	waitmovement 0
	msgbox Hideout_B2F_Text_EvilTeamAdminPostBattle, MSGBOX_DEFAULT
	setflag FLAG_EVIL_TEAM_ESCAPED_IN_SUBMARINE
	setflag FLAG_HIDE_GRUNTS_LILYCOVE
	release
	end

Hideout_B2F_Movement_SubmarineDepartLeft:: @ 815D958
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Hideout_B2F_Movement_SubmarineDepartRight:: @ 815D95D
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

AquaHideout_B2F_EventScript_EvilTeamGrunt2:: @ 815D962
MagmaHideout_B2F_EventScript_EvilTeamGrunt2:: @ 815D962
	trainerbattle_single TRAINER_HIDEOUT_B2F_GRUNT_2, Hideout_B2F_Text_EvilTeamGrunt2Intro, Hideout_B2F_Text_EvilTeamGrunt2Defeat, Hideout_B2F_EventScript_EvilTeamGrunt2Defeated
	msgbox Hideout_B2F_Text_EvilTeamGrunt2PostBattle, MSGBOX_AUTOCLOSE
	end

Hideout_B2F_EventScript_EvilTeamGrunt2Defeated:: @ 815D97D
	msgbox Hideout_B2F_Text_EvilTeamGrunt2PostBattle, MSGBOX_DEFAULT
	release
	end

AquaHideout_B2F_EventScript_EvilTeamGrunt3:: @ 815D987
MagmaHideout_B2F_EventScript_EvilTeamGrunt3:: @ 815D987
	trainerbattle_single TRAINER_HIDEOUT_B2F_GRUNT_3, Hideout_B2F_Text_EvilTeamGrunt3Intro, Hideout_B2F_Text_EvilTeamGrunt3Defeat
	msgbox Hideout_B2F_Text_EvilTeamGrunt3PostBattle, MSGBOX_AUTOCLOSE
	end
