SeafloorCavern_Entrance_MapScripts:: @ 815DA4A
	map_script MAP_SCRIPT_ON_RESUME, SeafloorCavern_Entrance_OnResume
	.byte 0

SeafloorCavern_Entrance_OnResume:: @ 815DA50
	setdivewarp MAP_UNDERWATER_SEAFLOOR_CAVERN, WARP_ID_NONE, 6, 5
	setescapewarp MAP_UNDERWATER_SEAFLOOR_CAVERN, 255, 6, 5
	end
