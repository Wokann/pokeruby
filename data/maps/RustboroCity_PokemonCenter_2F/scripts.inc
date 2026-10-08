RustboroCity_PokemonCenter_2F_MapScripts:: @ 8157BB1
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

RustboroCity_PokemonCenter_2F_EventScript_Colosseum:: @ 8157BC1
	call CableClub_EventScript_Colosseum
	end

RustboroCity_PokemonCenter_2F_EventScript_TradeCenter:: @ 8157BC7
	call CableClub_EventScript_TradeCenter
	end

RustboroCity_PokemonCenter_2F_EventScript_RecordCorner:: @ 8157BCD
	call CableClub_EventScript_RecordCorner
	end
