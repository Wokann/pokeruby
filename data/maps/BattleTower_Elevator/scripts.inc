BattleTower_Elevator_MapScripts:: @ 81607CF
	map_script MAP_SCRIPT_ON_FRAME_TABLE, BattleTower_Elevator_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleTower_Elevator_OnWarp
	.byte 0

BattleTower_Elevator_OnFrame:: @ 81607DA
	map_script_2 VAR_TEMP_0, FALSE, BattleTower_Elevator_EventScript_EnterElevator
	.2byte 0

BattleTower_Elevator_EventScript_EnterElevator:: @ 81607E4
	setvar VAR_TEMP_0, TRUE
	applymovement LOCALID_BATTLE_TOWER_ELEVATOR_ATTENDANT, BattleTower_Elevator_Movement_AttendantEnter
	applymovement LOCALID_PLAYER, BattleTower_Elevator_Movement_PlayerEnter
	waitmovement 0
	special MoveElevator
	delay 64
	applymovement LOCALID_BATTLE_TOWER_ELEVATOR_ATTENDANT, BattleTower_Elevator_Movement_AttendantExit
	applymovement LOCALID_PLAYER, BattleTower_Elevator_Movement_PlayerExit
	waitmovement 0
	setvar VAR_TEMP_0, FALSE
	warp MAP_BATTLE_TOWER_CORRIDOR, WARP_ID_NONE, 8, 1
	waitstate
	end

BattleTower_Elevator_OnWarp:: @ 8160821
	map_script_2 VAR_TEMP_1, FALSE, BattleTower_Elevator_EventScript_TurnPlayerNorth
	.2byte 0

BattleTower_Elevator_EventScript_TurnPlayerNorth:: @ 816082B
	setvar VAR_TEMP_1, TRUE
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

BattleTower_Elevator_Movement_AttendantEnter:: @ 8160835
	walk_up
	walk_right
	face_down
	step_end

BattleTower_Elevator_Movement_PlayerEnter:: @ 8160839
	walk_up
	walk_up
	face_down
	step_end

BattleTower_Elevator_Movement_AttendantExit:: @ 816083D
	walk_down
	walk_down
	set_invisible
	step_end

BattleTower_Elevator_Movement_PlayerExit:: @ 8160841
	walk_right
	walk_down
	walk_down
	step_end
