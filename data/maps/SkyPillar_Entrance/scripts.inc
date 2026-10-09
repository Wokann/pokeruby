SkyPillar_Entrance_MapScripts:: @ 815F2BD
	map_script MAP_SCRIPT_ON_TRANSITION, SkyPillar_Entrance_OnTransition
	.byte 0

SkyPillar_Entrance_OnTransition:: @ 815F2C3
	setflag FLAG_LANDMARK_SKY_PILLAR
	end
