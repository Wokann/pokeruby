AquaHideout_B2F_MapScripts:: @ 815D8B1
	map_script MAP_SCRIPT_ON_TRANSITION, AquaHideout_B2F_OnTransition
	.byte 0

AquaHideout_B2F_OnTransition:: @ 815D8B7
	call Common_EventScript_SetupEvilTeamGfxIds
	end

AquaHideout_B2F_EventScript_15D8BD:: @ 815D8BD
MagmaHideout_B2F_EventScript_15D8BD:: @ 815D8BD
	lockall
	setvar VAR_0x8008, 1
	playse SE_PIN
	applymovement VAR_0x8008, Common_Movement_ExclamationMark
	waitmovement 0
	.ifdef SAPPHIRE
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	.else
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFasterUp
	.endif
	waitmovement 0
	setvar VAR_TEMP_1, 1
	releaseall
	end

AquaHideout_B2F_EventScript_15D8E1:: @ 815D8E1
MagmaHideout_B2F_EventScript_15D8E1:: @ 815D8E1
	trainerbattle_single TRAINER_HIDEOUT_B2F_GRUNT_1, AquaHideout_B2F_Text_197093, AquaHideout_B2F_Text_197145, AquaHideout_B2F_EventScript_SubmarineEscape
	msgbox AquaHideout_B2F_Text_1971AE, MSGBOX_DEFAULT
	release
	end

AquaHideout_B2F_EventScript_SubmarineEscape:: @ 815D8FD
	setvar VAR_0x8008, 1
	.ifdef SAPPHIRE
	setvar VAR_0x8009, 4
	.else
	setvar VAR_0x8009, 3
	.endif
	.ifdef SAPPHIRE
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFasterLeft
	.else
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFasterRight
	.endif
	waitmovement 0
	delay 20
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	waitmovement 0
	msgbox AquaHideout_B2F_Text_OurBossGotThroughHisPreparations, MSGBOX_DEFAULT
	closemessage
	.ifdef SAPPHIRE
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFasterLeft
	.else
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFasterRight
	.endif
	.ifdef SAPPHIRE
	applymovement VAR_0x8009, AquaHideout_B2F_Movement_SumbarineDepartLeft
	.else
	applymovement VAR_0x8009, AquaHideout_B2F_Movement_SumbarineDepartRight
	.endif
	waitmovement 0
	removeobject VAR_0x8009
	delay 20
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	waitmovement 0
	msgbox AquaHideout_B2F_Text_1971AE, MSGBOX_DEFAULT
	setflag FLAG_EVIL_TEAM_ESCAPED_IN_SUBMARINE
	setflag FLAG_HIDE_GRUNTS_LILYCOVE
	release
	end

AquaHideout_B2F_Movement_SumbarineDepartLeft:: @ 815D958
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

AquaHideout_B2F_Movement_SumbarineDepartRight:: @ 815D95D
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

AquaHideout_B2F_EventScript_Grunt4:: @ 815D962
MagmaHideout_B2F_EventScript_15D962:: @ 815D962
	trainerbattle_single TRAINER_HIDEOUT_B2F_GRUNT_2, AquaHideout_B2F_Text_Grunt4Intro, AquaHideout_B2F_Text_Grunt4Defeat, AquaHideout_B2F_EventScript_Grunt4Defeated
	msgbox AquaHideout_B2F_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

AquaHideout_B2F_EventScript_Grunt4Defeated:: @ 815D97D
	msgbox AquaHideout_B2F_Text_Grunt4PostBattle, MSGBOX_DEFAULT
	release
	end

AquaHideout_B2F_EventScript_Grunt6:: @ 815D987
MagmaHideout_B2F_EventScript_15D987:: @ 815D987
	trainerbattle_single TRAINER_HIDEOUT_B2F_GRUNT_3, AquaHideout_B2F_Text_Grunt6Intro, AquaHideout_B2F_Text_Grunt6Defeat
	msgbox AquaHideout_B2F_Text_Grunt6PostBattle, MSGBOX_AUTOCLOSE
	end
