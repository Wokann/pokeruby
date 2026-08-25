FortreeCity_PokemonCenter_2F_MapScripts:: @ 8158044
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

FortreeCity_PokemonCenter_2F_EventScript_158054:: @ 8158054
	call CableClub_EventScript_Colosseum
	end

FortreeCity_PokemonCenter_2F_EventScript_15805A:: @ 815805A
	call CableClub_EventScript_TradeCenter
	end

FortreeCity_PokemonCenter_2F_EventScript_158060:: @ 8158060
	call CableClub_EventScript_RecordCorner
	end
