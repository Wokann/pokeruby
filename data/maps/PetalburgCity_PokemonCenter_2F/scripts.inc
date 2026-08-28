PetalburgCity_PokemonCenter_2F_MapScripts:: @ 815528D
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad
	.byte 0

PetalburgCity_PokemonCenter_2F_EventScript_15529D:: @ 815529D
	call CableClub_EventScript_Colosseum
	end

PetalburgCity_PokemonCenter_2F_EventScript_1552A3:: @ 81552A3
	call CableClub_EventScript_TradeCenter
	end

PetalburgCity_PokemonCenter_2F_EventScript_1552A9:: @ 81552A9
	call PetalburgCity_PokemonCenter_2F_EventScript_1A41FB
	end
