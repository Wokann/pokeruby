LavaridgeTown_PokemonCenter_2F_MapScripts:: @ 8153944
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

LavaridgeTown_PokemonCenter_2F_EventScript_Colosseum:: @ 8153954
	call CableClub_EventScript_Colosseum
	end

LavaridgeTown_PokemonCenter_2F_EventScript_TradeCenter:: @ 815395A
	call CableClub_EventScript_TradeCenter
	end

LavaridgeTown_PokemonCenter_2F_EventScript_RecordCorner:: @ 8153960
	call CableClub_EventScript_RecordCorner
	end
