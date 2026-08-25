MeteorFalls_1F_1R_MapScripts:: @ 815C490
	map_script MAP_SCRIPT_ON_TRANSITION, MeteorFalls_1F_1R_OnTransition
	.byte 0

MeteorFalls_1F_1R_OnTransition:: @ 815C496
	call Common_EventScript_SetupEvilTeamGfxIds
	end

MeteorFalls_1F_1R_EventScript_EvilTeamStealsMeteoriteScene:: @ 815C49C
	lockall
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 30
	applymovement 5, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_WithThisMeteorite, MSGBOX_DEFAULT
	closemessage
	applymovement 5, Common_Movement_WalkInPlaceFastestUp
	applymovement 6, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	playse SE_PIN
	applymovement 5, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 5, Common_Movement_Delay48
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_DontExpectMercyFromEvilTeam, MSGBOX_DEFAULT
	closemessage
	applymovement 5, MeteorFalls_1F_1R_Movement_EvilTeamGruntApproachPlayer
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_HoldItRightThereEvilTeam, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	applymovement 5, Common_Movement_WalkInPlaceFastestLeft
	applymovement 6, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	addobject 7
	addobject 8
	addobject 9
	applymovement 7, MeteorFalls_1F_1R_Movement_GoodTeamLeaderArrive
	applymovement 8, MeteorFalls_1F_1R_Movement_GoodTeamGrunt1Arrive
	applymovement 9, MeteorFalls_1F_1R_Movement_GoodTeamGrunt2Arrive
	waitmovement 0
	applymovement 5, Common_Movement_WalkInPlaceFastestLeft
	applymovement 6, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_BeSeeingYouOpposingEvilTeam, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, MeteorFalls_1F_1R_Movement_PlayerPushedByEvilTeam
	applymovement 5, MeteorFalls_1F_1R_Movement_EvilTeamGrunt1Exit
	applymovement 6, MeteorFalls_1F_1R_Movement_EvilTeamGrunt2Exit
	waitmovement 0
	removeobject 5
	removeobject 6
	applymovement 7, MeteorFalls_1F_1R_Movement_GoodTeamLeaderApproachPlayer
	waitmovement 0
	.ifdef SAPPHIRE
	msgbox MeteorFalls_1F_1R_Text_MaxieIntroducesMagma, MSGBOX_DEFAULT
	.else
	msgbox MeteorFalls_1F_1R_Text_ArchieIntroducesAqua, MSGBOX_DEFAULT
	.endif
	closemessage
	applymovement 8, MeteorFalls_1F_1R_Movement_GoodTeamGrunt1ApproachLeader
	applymovement 9, MeteorFalls_1F_1R_Movement_GoodTeamGrunt2ApproachLeader
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_GoodLeaderWeShouldChaseEvilTeam, MSGBOX_DEFAULT
	applymovement 7, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_GoodLeaderOrdersPursuit, MSGBOX_DEFAULT
	applymovement 7, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_GoodLeaderFarewell, MSGBOX_DEFAULT
	closemessage
	applymovement 7, MeteorFalls_1F_1R_Movement_GoodTeamLeaderExit
	applymovement 8, MeteorFalls_1F_1R_Movement_GoodTeamGrunt1Exit
	applymovement 9, MeteorFalls_1F_1R_Movement_GoodTeamGrunt2Exit
	waitmovement 0
	removeobject 7
	removeobject 8
	removeobject 9
	setflag FLAG_HIDE_GRUNTS_BLOCKING_CABLE_CAR
	setvar VAR_METEOR_FALLS_STATE, 1
	releaseall
	end

MeteorFalls_1F_1R_Movement_EvilTeamGruntApproachPlayer:: @ 815C5EE
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_EvilTeamGrunt1Exit:: @ 815C5F1
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

MeteorFalls_1F_1R_Movement_EvilTeamGrunt2Exit:: @ 815C5FC
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamLeaderArrive:: @ 815C608
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamGrunt1Arrive:: @ 815C60F
	delay_16
	delay_16
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamGrunt2Arrive:: @ 815C617
	delay_16
	delay_16
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamLeaderExit:: @ 815C61F
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamLeaderApproachPlayer:: @ 815C628
	walk_right
	walk_right
	walk_up
	walk_up
	walk_in_place_fastest_left
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamGrunt1Exit:: @ 815C62E
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamGrunt1ApproachLeader:: @ 815C639
	walk_right
	walk_right
	walk_right
	walk_in_place_fastest_up
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamGrunt2Exit:: @ 815C63E
	walk_up
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_GoodTeamGrunt2ApproachLeader:: @ 815C64A
	walk_right
	walk_right
	walk_right
	walk_in_place_fastest_up
	step_end

MeteorFalls_1F_1R_Movement_PlayerPushedByEvilTeam:: @ 815C64F
	walk_in_place_fastest_down
	delay_4
	walk_in_place_fastest_right
	lock_facing_direction
	walk_left
	unlock_facing_direction
	step_end

MeteorFalls_1F_1R_EventScript_ProfCozmo:: @ 815C656
	lock
	faceplayer
	goto_if_set FLAG_MET_PROF_COSMO, MeteorFalls_1F_1R_EventScript_MetProfCozmo
	setflag FLAG_MET_PROF_COSMO
	msgbox MeteorFalls_1F_1R_Text_MeetProfCozmo, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_1R_EventScript_MetProfCozmo:: @ 815C66E
	msgbox MeteorFalls_1F_1R_Text_WhatsEvilTeamDoingAtMtChimney, MSGBOX_DEFAULT
	release
	end
