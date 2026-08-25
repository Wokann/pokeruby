MtChimney_EventScript_OtherTeamLeader:: @ 81B2C95
	lock
	faceplayer
	call_if_unset FLAG_EVIL_LEADER_PLEASE_STOP, MtChimney_EventScript_OtherTeamLeaderGoStopEvilTeam
	call_if_set FLAG_EVIL_LEADER_PLEASE_STOP, MtChimney_EventScript_OtherTeamLeaderBusyFighting
	closemessage
	applymovement LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER, Common_Movement_FaceOriginalDirection
	waitmovement 0
	setflag FLAG_EVIL_LEADER_PLEASE_STOP
	release
	end

MtChimney_EventScript_OtherTeamLeaderGoStopEvilTeam:: @ 81B2CB9
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_MaxieGoStopTeamAqua, MSGBOX_DEFAULT
	.else
	msgbox MtChimney_Text_ArchieGoStopTeamMagma, MSGBOX_DEFAULT
	.endif
	return

MtChimney_EventScript_OtherTeamLeaderBusyFighting:: @ 81B2CC2
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_MaxieIHaveMyHandsFull, MSGBOX_DEFAULT
	.else
	msgbox MtChimney_Text_ArchieIHaveMyHandsFull, MSGBOX_DEFAULT
	.endif
	return

MtChimney_EventScript_EvilTeamLeader:: @ 81B2CCB
	lockall
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_ArchieStopVolcanoPlan, MSGBOX_DEFAULT
	.else
	msgbox MtChimney_Text_MaxieIntensifyVolcanoPlan, MSGBOX_DEFAULT
	.endif
	applymovement LOCALID_MT_CHIMNEY_EVIL_TEAM_LEADER, Common_Movement_FacePlayer
	waitmovement 0
	playse SE_PIN
	applymovement LOCALID_MT_CHIMNEY_EVIL_TEAM_LEADER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_MT_CHIMNEY_EVIL_TEAM_LEADER, Common_Movement_Delay48
	waitmovement 0
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_ArchieIntro, MSGBOX_DEFAULT
	.else
	msgbox MtChimney_Text_MaxieIntro, MSGBOX_DEFAULT
	.endif
	.ifdef SAPPHIRE
	trainerbattle_no_intro TRAINER_MT_CHIMNEY_GRUNT_3, MtChimney_Text_ArchieDefeat
	.else
	trainerbattle_no_intro TRAINER_MT_CHIMNEY_GRUNT_3, MtChimney_Text_MaxieDefeat
	.endif
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_ArchieRetreat, MSGBOX_DEFAULT
	.else
	msgbox MtChimney_Text_MaxieRetreat, MSGBOX_DEFAULT
	.endif
	closemessage
	delay 30
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_MT_CHIMNEY_EVIL_TEAM_LEADER
	removeobject LOCALID_MT_CHIMNEY_EVIL_TEAM_ADMIN
	removeobject LOCALID_MT_CHIMNEY_EVIL_TEAM_GRUNT
	setflag FLAG_HIDE_EVIL_TEAM_MT_CHIMNEY
	fadescreen FADE_FROM_BLACK
	setobjectxyperm LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER, 10, 12
	addobject LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER
	compare VAR_FACING, DIR_EAST
	call_if_eq MtChimney_EventScript_OtherTeamLeaderApproachPlayerEast
	compare VAR_FACING, DIR_NORTH
	call_if_eq MtChimney_EventScript_OtherTeamLeaderApproachPlayerNorth
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_MaxieThankYou, MSGBOX_DEFAULT
	.else
	msgbox MtChimney_Text_ArchieThankYou, MSGBOX_DEFAULT
	.endif
	closemessage
	compare VAR_FACING, DIR_EAST
	call_if_eq MtChimney_EventScript_OtherTeamLeaderExitEast
	compare VAR_FACING, DIR_NORTH
	call_if_eq MtChimney_EventScript_OtherTeamLeaderExitNorth
	removeobject LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER
	setflag FLAG_HIDE_OTHER_TEAM_MT_CHIMNEY
	setflag FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY
	clearflag FLAG_HIDE_PROF_COSMO_FALLARBOR
	setflag FLAG_HIDE_PROF_COSMO_METEOR_FALLS
	releaseall
	end

MtChimney_EventScript_OtherTeamLeaderApproachPlayerEast:: @ 81B2D7D
	applymovement LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER, MtChimney_Movement_OtherTeamLeaderApproachPlayerEast
	waitmovement 0
	return

MtChimney_EventScript_OtherTeamLeaderApproachPlayerNorth:: @ 81B2D88
	applymovement LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER, MtChimney_Movement_OtherTeamLeaderApproachPlayerNorth
	waitmovement 0
	return

MtChimney_EventScript_OtherTeamLeaderExitEast:: @ 81B2D93
	applymovement LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER, MtChimney_Movement_OtherTeamLeaderExitEast
	waitmovement 0
	return

MtChimney_EventScript_OtherTeamLeaderExitNorth:: @ 81B2D9E
	applymovement LOCALID_MT_CHIMNEY_OTHER_TEAM_LEADER, MtChimney_Movement_OtherTeamLeaderExitNorth
	waitmovement 0
	return
