EverGrandeCity_Hall1_MapScripts:: @ 815BA0C
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, EverGrandeCity_Hall1_OnWarp
	.byte 0

EverGrandeCity_Hall1_OnWarp:: @ 815BA12
	map_script_2 VAR_TEMP_1, 0, EverGrandeCity_Hall1_EventScript_TurnPlayerNorth
	.2byte 0

EverGrandeCity_Hall1_EventScript_TurnPlayerNorth:: @ 815BA1C
	turnobject LOCALID_PLAYER, 2
	end
