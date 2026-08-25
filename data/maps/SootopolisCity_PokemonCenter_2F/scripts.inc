SootopolisCity_PokemonCenter_2F_MapScripts:: @ 815B1F8
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

SootopolisCity_PokemonCenter_2F_EventScript_Colosseum:: @ 815B208
	call CableClub_EventScript_Colosseum
	end

SootopolisCity_PokemonCenter_2F_EventScript_TradeCenter:: @ 815B20E
	call CableClub_EventScript_TradeCenter
	end

SootopolisCity_PokemonCenter_2F_EventScript_RecordCorner:: @ 815B214
	call CableClub_EventScript_RecordCorner
	end
