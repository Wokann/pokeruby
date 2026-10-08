FallarborTown_PokemonCenter_2F_MapScripts:: @ 8153B8C
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

FallarborTown_PokemonCenter_2F_EventScript_Colosseum:: @ 8153B9C
	call CableClub_EventScript_Colosseum
	end

FallarborTown_PokemonCenter_2F_EventScript_TradeCenter:: @ 8153BA2
	call CableClub_EventScript_TradeCenter
	end

FallarborTown_PokemonCenter_2F_EventScript_RecordCorner:: @ 8153BA8
	call CableClub_EventScript_RecordCorner
	end
