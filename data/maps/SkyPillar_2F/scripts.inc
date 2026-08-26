SkyPillar_2F_MapScripts:: @ 815F2C9
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CaveHole_CheckFallDownHole
	map_script MAP_SCRIPT_ON_TRANSITION, CaveHole_FixCrackedGround
	map_script MAP_SCRIPT_ON_RESUME, SkyPillar_2F_SetHoleWarp
	.byte 0

SkyPillar_2F_SetHoleWarp:: @ 815F2D9
	setstepcallback STEP_CB_CRACKED_FLOOR
	setholewarp MAP_SKY_PILLAR_1F, 255, 0, 0
	end
