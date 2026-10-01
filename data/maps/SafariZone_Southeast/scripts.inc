SafariZone_Southeast_MapScripts:: @ 8160036
	map_script MAP_SCRIPT_ON_TRANSITION, SafariZone_Southeast_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SafariZone_Southeast_OnFrame
	.byte 0

SafariZone_Southeast_OnFrame:: @ 8160041
	map_script_2 VAR_SAFARI_ZONE_STATE, 2, SafariZone_Southeast_EventScript_EnterSafariZone
	.2byte 0

SafariZone_Southeast_EventScript_EnterSafariZone:: @ 816004B
	lockall
	setvar VAR_TEMP_1, FALSE
	applymovement LOCALID_PLAYER, SafariZone_Southeast_Movement_PlayerEnter
	waitmovement 0
	applymovement LOCALID_SAFARI_EXIT_ATTENDANT, SafariZone_Southeast_Movement_ExitAttendantBlockDoor
	waitmovement 0
	setobjectxyperm LOCALID_SAFARI_EXIT_ATTENDANT, 32, 34
	setvar VAR_SAFARI_ZONE_STATE, 0
	releaseall
	end

SafariZone_Southeast_OnTransition:: @ 8160073
	compare VAR_SAFARI_ZONE_STATE, 2
	call_if_eq SafariZone_Southeast_EventScript_SetExitAttendantAside
	end

SafariZone_Southeast_EventScript_SetExitAttendantAside:: @ 816007F
	setobjectxyperm LOCALID_SAFARI_EXIT_ATTENDANT, 31, 34
	return

SafariZone_Southeast_Movement_PlayerEnter:: @ 8160087
	walk_down
	step_end

SafariZone_Southeast_Movement_ExitAttendantBlockDoor:: @ 8160089
	walk_right
	walk_in_place_faster_down
	step_end

SafariZone_Southeast_EventScript_Boy:: @ 816008C
	msgbox SafariZone_Southeast_Text_Boy, MSGBOX_NPC
	end

SafariZone_Southeast_EventScript_Man:: @ 8160095
	msgbox SafariZone_Southeast_Text_Man, MSGBOX_NPC
	end

SafariZone_Southeast_EventScript_Youngster:: @ 816009E
	msgbox SafariZone_Southeast_Text_Youngster, MSGBOX_NPC
	end

SafariZone_Southeast_EventScript_ExitAttendant:: @ 81600A7
	lock
	faceplayer
	compare VAR_TEMP_1, FALSE
	goto_if_eq SafariZone_Southeast_EventScript_GoodLuck
	msgbox SafariZone_Southeast_Text_StillHaveTimeExit, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SafariZone_Southeast_EventScript_ExitEarly
	msgbox SafariZone_Southeast_Text_EnjoyTheRestOfYourAdventure, MSGBOX_DEFAULT
	release
	end

SafariZone_Southeast_EventScript_GoodLuck:: @ 81600D1
	setvar VAR_TEMP_1, TRUE
	msgbox SafariZone_Southeast_Text_GoodLuck, MSGBOX_DEFAULT
	release
	end

SafariZone_Southeast_EventScript_ExitEarly:: @ 81600E0
	msgbox SafariZone_Southeast_Text_ExitEarlyThankYouForPlaying, MSGBOX_DEFAULT
	closemessage
	switch VAR_FACING
	case DIR_NORTH, SafariZone_Southeast_EventScript_ExitEarlyNorth
	case DIR_EAST, SafariZone_Southeast_EventScript_ExitEarlyEast
	end

SafariZone_Southeast_EventScript_ExitEarlyNorth:: @ 8160105
	applymovement LOCALID_SAFARI_EXIT_ATTENDANT, SafariZone_Southeast_Movement_MoveExitAttendantNorth
	waitmovement 0
	applymovement LOCALID_PLAYER, SafariZone_Southeast_Movement_PlayerExitNorth
	waitmovement 0
	goto SafariZone_Southeast_EventScript_Exit
	end

SafariZone_Southeast_EventScript_ExitEarlyEast:: @ 816011F
	applymovement LOCALID_SAFARI_EXIT_ATTENDANT, SafariZone_Southeast_Movement_MoveExitAttendantEast
	waitmovement 0
	applymovement LOCALID_PLAYER, SafariZone_Southeast_Movement_PlayerExitEast
	waitmovement 0
	goto SafariZone_Southeast_EventScript_Exit
	end

SafariZone_Southeast_EventScript_Exit:: @ 8160139
	setvar VAR_SAFARI_ZONE_STATE, 1
	special ExitSafariMode
	warpdoor MAP_ROUTE121_SAFARI_ZONE_ENTRANCE, WARP_ID_NONE, 2, 5
	waitstate
	end

SafariZone_Southeast_Movement_PlayerExitNorth:: @ 816014B
	walk_up
	step_end

SafariZone_Southeast_Movement_PlayerExitEast:: @ 816014D
	walk_right
	walk_in_place_faster_up
	step_end

SafariZone_Southeast_Movement_MoveExitAttendantNorth:: @ 8160150
	walk_left
	walk_in_place_faster_right
	step_end

SafariZone_Southeast_Movement_MoveExitAttendantEast:: @ 8160153
	walk_down
	walk_in_place_faster_up
	step_end
