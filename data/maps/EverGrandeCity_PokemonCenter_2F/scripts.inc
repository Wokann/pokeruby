EverGrandeCity_PokemonCenter_2F_MapScripts:: @ 815BC90
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

EverGrandeCity_PokemonCenter_2F_EventScript_Colosseum:: @ 815BCA0
	call CableClub_EventScript_Colosseum
	end

EverGrandeCity_PokemonCenter_2F_EventScript_TradeCenter:: @ 815BCA6
	call CableClub_EventScript_TradeCenter
	end

EverGrandeCity_PokemonCenter_2F_EventScript_RecordCorner:: @ 815BCAC
	call CableClub_EventScript_RecordCorner
	end
