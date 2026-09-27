ScorchedSlab_MapScripts:: @ 815F29D
	map_script MAP_SCRIPT_ON_TRANSITION, ScorchedSlab_OnTransition
	.byte 0

ScorchedSlab_OnTransition:: @ 815F2A3
	setflag FLAG_LANDMARK_SCORCHED_SLAB
	end
