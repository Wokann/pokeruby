SouthernIsland_Interior_MapScripts:: @ 8160B53
	map_script MAP_SCRIPT_ON_RESUME, SouthernIsland_Interior_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, SouthernIsland_Interior_OnTransition
	.byte 0

SouthernIsland_Interior_OnResume:: @ 8160B5E
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, SouthernIsland_Interior_EventScript_TryRemoveLati
	end

SouthernIsland_Interior_EventScript_TryRemoveLati:: @ 8160B68
	removeobject 2
	return

SouthernIsland_Interior_OnTransition:: @ 8160B6C
	.ifdef SAPPHIRE
	setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_LATIOS
	.else
	setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_LATIAS
	.endif
	call SouthernIsland_Interior_EventScript_SetUpPlayerGfx
	end

SouthernIsland_Interior_EventScript_SetUpPlayerGfx:: @ 8160B77
	checkplayergender
	compare VAR_RESULT, 0
	goto_if_eq SouthernIsland_Interior_EventScript_SetBrendanGfx
	compare VAR_RESULT, 1
	goto_if_eq SouthernIsland_Interior_EventScript_SetMayGfx
	end

SouthernIsland_Interior_EventScript_SetBrendanGfx:: @ 8160B8F
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

SouthernIsland_Interior_EventScript_SetMayGfx:: @ 8160B95
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return

SouthernIsland_Interior_EventScript_TryLatiEncounter:: @ 8160B9B
	lockall
	setvar VAR_0x8008, 12
	goto SouthernIsland_Interior_EventScript_Lati
	end

SouthernIsland_Interior_EventScript_Lati:: @ 8160BA7
	goto_if_set FLAG_ENCOUNTERED_LATIAS_OR_LATIOS, SouthernIsland_Interior_EventScript_Sign
	goto_if_unset FLAG_SYS_HAS_EON_TICKET, SouthernIsland_Interior_EventScript_Sign
	setflag FLAG_ENCOUNTERED_LATIAS_OR_LATIOS
	special SpawnCameraDummy
	applymovement 127, SouthernIsland_Interior_Movement_CameraPanUp
	waitmovement 0
	delay 50
	waitse
	playmoncry SPECIES_LATIAS_OR_LATIOS, 0
	delay 30
	waitmoncry
	addobject 2
	delay 30
	applymovement 127, SouthernIsland_Interior_Movement_CameraPanDown
	applymovement 2, SouthernIsland_Interior_Movement_LatiApproach
	waitmovement 0
	delay 50
	special RemoveCameraDummy
	setwildbattle SPECIES_LATIAS_OR_LATIOS, 50, ITEM_SOUL_DEW
	setflag FLAG_HIDE_LATIOS_OR_LATIAS_STATIONARY
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartSouthernIslandBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	releaseall
	end

SouthernIsland_Interior_EventScript_Sign:: @ 8160C08
	msgbox SouthernIsland_Interior_Text_Sign, MSGBOX_DEFAULT
	releaseall
	end

SouthernIsland_Interior_Movement_CameraPanUp:: @ 8160C12
	walk_up
	walk_up
	walk_up
	step_end

SouthernIsland_Interior_Movement_CameraPanDown:: @ 8160C16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	walk_down
	walk_down
	walk_down
	walk_in_place_fastest_up
	step_end

SouthernIsland_Interior_Movement_LatiApproach:: @ 8160C22
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	delay_16
	delay_16
	walk_down
	walk_down
	walk_down
	walk_down
	step_end
