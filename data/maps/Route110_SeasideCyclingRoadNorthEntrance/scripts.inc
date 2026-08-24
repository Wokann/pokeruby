Route110_SeasideCyclingRoadNorthEntrance_MapScripts:: @ 81634C8
	map_script MAP_SCRIPT_ON_TRANSITION, Route110_SeasideCyclingRoadNorthEntrance_OnTransition
	.byte 0

Route110_SeasideCyclingRoadNorthEntrance_OnTransition:: @ 81634CE
	compare VAR_CYCLING_CHALLENGE_STATE, 3
	call_if_eq Route110_SeasideCyclingRoadNorthEntrance_EventScript_RestartChallenge
	compare VAR_CYCLING_CHALLENGE_STATE, 2
	call_if_eq Route110_SeasideCyclingRoadNorthEntrance_EventScript_RestartChallenge
	end

Route110_SeasideCyclingRoadNorthEntrance_EventScript_RestartChallenge:: @ 81634E5
	setvar VAR_CYCLING_CHALLENGE_STATE, 1
	return

Route110_SeasideCyclingRoadNorthEntrance_EventScript_Clerk:: @ 81634EB
	lock
	faceplayer
	msgbox Route110_SeasideCyclingRoadNorthEntrance_Text_GoAllOutOnCyclingRoad, MSGBOX_DEFAULT
	release
	end

Route110_SeasideCyclingRoadNorthEntrance_EventScript_BikeCheck:: @ 81634F7
@ Coord event directly in front of the gatesman heading left to right
	lockall
	specialvar VAR_RESULT, GetPlayerAvatarBike @ player state?
	compare VAR_RESULT, 2
	call_if_eq Route110_SeasideCyclingRoadNorthEntrance_EventScript_OnMachBike
	compare VAR_RESULT, 0
	goto_if_eq Route110_SeasideCyclingRoadNorthEntrance_EventScript_NoBike
	setflag FLAG_SYS_CYCLING_ROAD
	setvar VAR_TEMP_1, 1
	releaseall
	end

Route110_SeasideCyclingRoadNorthEntrance_EventScript_OnMachBike:: @ 816351D
@ You're on the Mach Bike!
	setvar VAR_CYCLING_CHALLENGE_STATE, 1
	return

Route110_SeasideCyclingRoadNorthEntrance_EventScript_NoBike:: @ 8163523
@ You don't have a bike!
	msgbox Route110_SeasideCyclingRoad_Text_TooDangerousToWalk, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route110_SeasideCyclingRoadNorthEntrance_Movement_PushPlayerBackFromCounter
	waitmovement 0
	releaseall
	end

Route110_SeasideCyclingRoadNorthEntrance_Movement_PushPlayerBackFromCounter:: @ 8163538
	walk_left
	step_end

Route110_SeasideCyclingRoadNorthEntrance_EventScript_ClearCyclingRoad:: @ 816353A
@ Coord event directly in front of the gatesman heading right to left
	lockall
	setvar VAR_CYCLING_CHALLENGE_STATE, 0
	clearflag FLAG_SYS_CYCLING_ROAD
	setvar VAR_TEMP_1, 0
	releaseall
	end
