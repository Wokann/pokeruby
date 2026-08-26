Underwater_SootopolisCity_MapScripts:: @ 815CAE4
	map_script MAP_SCRIPT_ON_RESUME, Underwater_SootopolisCity_OnResume
	.byte 0

Underwater_SootopolisCity_OnResume:: @ 815CAEA
	setdivewarp MAP_SOOTOPOLIS_CITY, WARP_ID_NONE, 29, 53
	end
