SkyPillar_4F_MapScripts:: @ 815F2E5
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CaveHole_CheckFallDownHole
	map_script MAP_SCRIPT_ON_TRANSITION, CaveHole_FixCrackedGround
	map_script MAP_SCRIPT_ON_RESUME, SkyPillar_4F_SetHoleWarp
	.byte 0

SkyPillar_4F_SetHoleWarp:: @ 815F2F5
	setstepcallback STEP_CB_CRACKED_FLOOR
	setholewarp MAP_SKY_PILLAR_3F, 255, 0, 0
	end
