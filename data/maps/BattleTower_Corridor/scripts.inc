BattleTower_Corridor_MapScripts:: @ 8160845
	map_script MAP_SCRIPT_ON_LOAD, BattleTower_Corridor_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, BattleTower_Corridor_OnFrame
	.byte 0

BattleTower_Corridor_OnLoad:: @ 8160850
	compare VAR_0x8006, 1
	goto_if_eq BattleTower_Corridor_EventScript_OpenFarDoor
	setmetatile 12, 0, METATILE_BattleTower_CorridorOpenDoor_Top, 0
	setmetatile 12, 1, METATILE_BattleTower_CorridorOpenDoor_Bottom, 0
	end

BattleTower_Corridor_EventScript_OpenFarDoor:: @ 816086E
	setmetatile 15, 0, METATILE_BattleTower_CorridorOpenDoor_Top, 0
	setmetatile 15, 1, METATILE_BattleTower_CorridorOpenDoor_Bottom, 0
	end

BattleTower_Corridor_OnFrame:: @ 8160881
	map_script_2 VAR_TEMP_0, 0, BattleTower_Corridor_EventScript_EnterCorridor
	.2byte 0

BattleTower_Corridor_EventScript_EnterCorridor:: @ 816088B
	setvar VAR_TEMP_0, 1
	compare VAR_0x8006, 1
	goto_if_eq BattleTower_Corridor_EventScript_WalkToFarDoor
	applymovement 1, BattleTower_Corridor_Movement_AttendantWalkToDoor
	applymovement LOCALID_PLAYER, BattleTower_Corridor_Movement_PlayerWalkToDoor
	waitmovement 0
	goto BattleTower_Corridor_EventScript_WarpToBattleRoom

BattleTower_Corridor_EventScript_WalkToFarDoor:: @ 81608B1
	applymovement 1, BattleTower_Corridor_Movement_AttendantWalkToFarDoor
	applymovement LOCALID_PLAYER, BattleTower_Corridor_Movement_PlayerWalkToFarDoor
	waitmovement 0

BattleTower_Corridor_EventScript_WarpToBattleRoom:: @ 81608C2
	setvar VAR_TEMP_0, 0
	warp MAP_BATTLE_TOWER_BATTLE_ROOM, 255, 4, 8
	waitstate
	end

BattleTower_Corridor_Movement_PlayerWalkToFarDoor:: @ 81608D1
	walk_right

BattleTower_Corridor_Movement_AttendantWalkToFarDoor:: @ 81608D2
	walk_right
	walk_right

BattleTower_Corridor_Movement_PlayerWalkToDoor:: @ 81608D4
	walk_right

BattleTower_Corridor_Movement_AttendantWalkToDoor:: @ 81608D5
	walk_right
	walk_right
	walk_right
	walk_up
	set_invisible
	step_end
