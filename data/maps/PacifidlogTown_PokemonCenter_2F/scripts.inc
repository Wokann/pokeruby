PacifidlogTown_PokemonCenter_2F_MapScripts:: @ 8154132
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

PacifidlogTown_PokemonCenter_2F_EventScript_UnusedColosseum:: @ 8154142
	call CableClub_EventScript_Colosseum
	end

PacifidlogTown_PokemonCenter_2F_EventScript_UnusedTradeCenter:: @ 8154148
	call CableClub_EventScript_TradeCenter
	end

PacifidlogTown_PokemonCenter_2F_EventScript_RecordCorner:: @ 815414E
	call CableClub_EventScript_RecordCorner
	end
