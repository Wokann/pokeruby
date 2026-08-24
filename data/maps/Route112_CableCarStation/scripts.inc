Route112_CableCarStation_MapScripts:: @ 815C033
	map_script MAP_SCRIPT_ON_TRANSITION, Route112_CableCarStation_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route112_CableCarStation_OnFrame
	.byte 0

Route112_CableCarStation_OnTransition:: @ 815C03E
	setescapewarp MAP_ROUTE112, 255, 28, 28
	compare VAR_CABLE_CAR_STATION_STATE, 2
	call_if_eq Route112_CableCarStation_EventScript_MoveAttendantAside
	end

Route112_CableCarStation_EventScript_MoveAttendantAside:: @ 815C052
	setobjectxyperm 1, 7, 4
	setobjectmovementtype 1, 9
	return

Route112_CableCarStation_OnFrame:: @ 815C05E
	map_script_2 VAR_CABLE_CAR_STATION_STATE, 2, Route112_CableCarStation_EventScript_ExitCableCar
	.2byte 0

Route112_CableCarStation_EventScript_ExitCableCar:: @ 815C068
	lockall
	applymovement LOCALID_PLAYER, Route112_CableCarStation_Movement_ExitCableCar
	applymovement 1, Route112_CableCarStation_Movement_FollowPlayerOutFromCableCar
	waitmovement 0
	setvar VAR_CABLE_CAR_STATION_STATE, 0
	setobjectxyperm 1, 6, 7
	setobjectmovementtype 1, 8
	releaseall
	end

Route112_CableCarStation_EventScript_Attendant:: @ 815C08C
	lock
	faceplayer
	msgbox Route112_CableCarStation_Text_CableCarReadyGetOn, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route112_CableCarStation_EventScript_RideCableCar
	compare VAR_RESULT, NO
	goto_if_eq Route112_CableCarStation_EventScript_DeclineRide
	end

Route112_CableCarStation_EventScript_RideCableCar:: @ 815C0AD
	msgbox MtChimney_CableCarStation_Text_StepThisWay, MSGBOX_DEFAULT
	closemessage
	applymovement 1, Route112_CableCarStation_Movement_LeadPlayerToCableCar
	applymovement LOCALID_PLAYER, Route112_CableCarStation_Movement_BoardCableCar
	waitmovement 0
	setvar VAR_0x8004, 0
	setvar VAR_CABLE_CAR_STATION_STATE, 1
	incrementgamestat GAME_STAT_RODE_CABLE_CAR
	special CableCarWarp
	special CableCar
	waitstate
	release
	end

Route112_CableCarStation_EventScript_DeclineRide:: @ 815C0DC
	msgbox Route112_CableCarStation_Text_RideAnotherTime, MSGBOX_DEFAULT
	release
	end

Route112_CableCarStation_Movement_LeadPlayerToCableCar:: @ 815C0E6
	walk_up
	walk_up
	walk_right
	walk_in_place_fastest_left
	step_end

Route112_CableCarStation_Movement_FollowPlayerOutFromCableCar:: @ 815C0EB
	delay_16
	walk_left
	walk_down
	walk_down
	step_end

Route112_CableCarStation_Movement_BoardCableCar:: @ 815C0F0
	walk_up
	walk_up
	walk_up
	delay_16
	step_end

Route112_CableCarStation_Movement_ExitCableCar:: @ 815C0F5
	walk_down
	walk_down
	walk_down
	delay_16
	step_end
