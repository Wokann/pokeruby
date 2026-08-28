Route117_PokemonDayCare_MapScripts:: @ 815C319
	map_script MAP_SCRIPT_ON_TRANSITION, Route117_PokemonDayCare_OnTransition
	.byte 0

Route117_PokemonDayCare_OnTransition:: @ 815C31F
	setflag FLAG_LANDMARK_POKEMON_DAYCARE
	end
