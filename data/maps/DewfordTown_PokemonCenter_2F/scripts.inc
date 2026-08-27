DewfordTown_PokemonCenter_2F_MapScripts:: @ 8153043
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

DewfordTown_PokemonCenter_2F_EventScript_Colosseum:: @ 8153053
	call CableClub_EventScript_Colosseum
	end

DewfordTown_PokemonCenter_2F_EventScript_TradeCenter:: @ 8153059
	call CableClub_EventScript_TradeCenter
	end

DewfordTown_PokemonCenter_2F_EventScript_RecordCorner:: @ 815305F
	call CableClub_EventScript_RecordCorner
	end
