LilycoveCity_PokemonCenter_2F_MapScripts:: @ 8159367
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

LilycoveCity_PokemonCenter_2F_EventScript_Colosseum:: @ 8159377
	call CableClub_EventScript_Colosseum
	end

LilycoveCity_PokemonCenter_2F_EventScript_TradeCenter:: @ 815937D
	call CableClub_EventScript_TradeCenter
	end

LilycoveCity_PokemonCenter_2F_EventScript_RecordCorner:: @ 8159383
	call CableClub_EventScript_RecordCorner
	end
