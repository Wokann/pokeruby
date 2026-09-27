MagmaHideout_B1F_MapScripts:: @ 815F2A8
	map_script MAP_SCRIPT_ON_RESUME, MagmaHideout_B1F_OnResume
	.byte 0

MagmaHideout_B1F_OnResume:: @ 815F2AE
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, MagmaHideout_B1F_EventScript_RemoveDefeatedObject
	end

MagmaHideout_B1F_EventScript_RemoveDefeatedObject:: @ 815F2B8
	removeobject VAR_LAST_TALKED
	return
