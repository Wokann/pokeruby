VerdanturfTown_PokemonCenter_2F_MapScripts:: @ 8153F38
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

PacifidlogTown_PokemonCenter_2F_EventScript_Colosseum:: @ 8153F48
VerdanturfTown_PokemonCenter_2F_EventScript_Colosseum:: @ 8153F48
	call CableClub_EventScript_Colosseum
	end

PacifidlogTown_PokemonCenter_2F_EventScript_TradeCenter:: @ 8153F4E
VerdanturfTown_PokemonCenter_2F_EventScript_TradeCenter:: @ 8153F4E
	call CableClub_EventScript_TradeCenter
	end

VerdanturfTown_PokemonCenter_2F_EventScript_RecordCorner:: @ 8153F54
	call CableClub_EventScript_RecordCorner
	end
