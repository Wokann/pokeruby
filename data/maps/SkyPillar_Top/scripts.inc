SkyPillar_Top_MapScripts:: @ 815F302
	map_script MAP_SCRIPT_ON_RESUME, SkyPillar_Top_OnResume
	.byte 0

SkyPillar_Top_OnResume:: @ 815F308
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, SkyPillar_Top_EventScript_TryRemoveRayquaza
	end

SkyPillar_Top_EventScript_TryRemoveRayquaza:: @ 815F312
	removeobject VAR_LAST_TALKED
	return

SkyPillar_Top_EventScript_Rayquaza:: @ 815F316
	lock
	faceplayer
	setwildbattle SPECIES_RAYQUAZA, 70, ITEM_NONE
	waitse
	playmoncry SPECIES_RAYQUAZA, 2
	delay 40
	waitmoncry
	setflag FLAG_HIDE_RAYQUAZA
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartRayquazaBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	release
	end
