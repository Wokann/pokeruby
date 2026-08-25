SlateportCity_PokemonCenter_2F_MapScripts:: @ 81563EE
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

SlateportCity_PokemonCenter_2F_EventScript_Colosseum:: @ 81563FE
	call CableClub_EventScript_Colosseum
	end

SlateportCity_PokemonCenter_2F_EventScript_TradeCenter:: @ 8156404
	call CableClub_EventScript_TradeCenter
	end

SlateportCity_PokemonCenter_2F_EventScript_RecordCorner:: @ 815640A
	call CableClub_EventScript_RecordCorner
	end
