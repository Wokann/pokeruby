FieryPath_MapScripts:: @ 815D318
	map_script MAP_SCRIPT_ON_TRANSITION, FieryPath_OnTransition
	.byte 0

FieryPath_OnTransition:: @ 815D31E
	setflag FLAG_LANDMARK_FIERY_PATH
	end
