SSTidalCorridor_MapScripts:: @ 815FC94
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SSTidalCorridor_MapScript2_15FC9A
	.byte 0

SSTidalCorridor_MapScript2_15FC9A:: @ 815FC9A
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_BOARD_SLATEPORT, SSTidalCorridor_EventScript_15FCBC
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_BOARD_LILYCOVE, SSTidalCorridor_EventScript_15FCD2
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_EXIT_CURRENTS_RIGHT, SSTidalCorridor_EventScript_15FD24
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_EXIT_CURRENTS_LEFT, SSTidalCorridor_EventScript_15FD3A
	.2byte 0

SSTidalCorridor_EventScript_15FCBC:: @ 815FCBC
	special SetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_SLATEPORT
	lockall
	playse SE_DING_DONG
	msgbox SSTidalCorridor_Text_199007, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_15FCD2:: @ 815FCD2
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_LILYCOVE
	lockall
	playse SE_DING_DONG
	msgbox SSTidalCorridor_Text_199088, MSGBOX_DEFAULT
	releaseall
	end

SSTidalRooms_EventScript_15FCE5:: @ 815FCE5
	special SetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_SLATEPORT
	playse SE_DING_DONG
	msgbox SSTidalRooms_Text_199007, MSGBOX_DEFAULT
	return

SSTidalRooms_EventScript_15FCF9:: @ 815FCF9
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_LILYCOVE
	playse SE_DING_DONG
	msgbox SSTidalRooms_Text_1990F8, MSGBOX_DEFAULT
	return

gUnknown_0815FD0D:: @ 815FD0D
	compare VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_SLATEPORT
	goto_if_eq SSTidalCorridor_EventScript_15FD24
	compare VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_SLATEPORT
	goto_if_eq SSTidalCorridor_EventScript_15FD3A
	end

SSTidalCorridor_EventScript_15FD24:: @ 815FD24
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_LILYCOVE
	lockall
	playse SE_DING_DONG
	msgbox SSTidalCorridor_Text_199088, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_15FD3A:: @ 815FD3A
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_SLATEPORT
	lockall
	playse SE_DING_DONG
	msgbox SSTidalCorridor_Text_1990B4, MSGBOX_DEFAULT
	releaseall
	end

SSTidalRooms_EventScript_15FD50:: @ 815FD50
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_SLATEPORT
	playse SE_DING_DONG
	msgbox SSTidalRooms_Text_1990B4, MSGBOX_DEFAULT
	return

SSTidalRooms_EventScript_15FD64:: @ 815FD64
	switch VAR_SS_TIDAL_STATE
	case SS_TIDAL_DEPART_SLATEPORT, SSTidalRooms_EventScript_15FCF9
	case SS_TIDAL_HALFWAY_LILYCOVE, SSTidalRooms_EventScript_15FCF9
	case SS_TIDAL_DEPART_LILYCOVE, SSTidalRooms_EventScript_15FCE5
	case SS_TIDAL_HALFWAY_SLATEPORT, SSTidalRooms_EventScript_15FD50
	return

SSTidalCorridor_EventScript_15FD96:: @ 815FD96
	msgbox SSTidalCorridor_Text_199291, MSGBOX_NPC
	end

SSTidalCorridor_EventScript_15FD9F:: @ 815FD9F
	lock
	faceplayer
	waitse
	playmoncry SPECIES_WINGULL, 0
	msgbox SSTidalCorridor_Text_199388, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SSTidalCorridor_EventScript_15FDB2:: @ 815FDB2
	msgbox SSTidalCorridor_Text_1993A1, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_15FDBB:: @ 815FDBB
	msgbox SSTidalCorridor_Text_1993A9, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_15FDC4:: @ 815FDC4
	msgbox SSTidalCorridor_Text_1993B1, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_15FDCD:: @ 815FDCD
	msgbox SSTidalCorridor_Text_1993B9, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_15FDD6:: @ 815FDD6
	lock
	faceplayer
	compare VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_LILYCOVE
	goto_if_eq SSTidalCorridor_EventScript_15FDF8
	compare VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_SLATEPORT
	goto_if_eq SSTidalCorridor_EventScript_15FE17
	msgbox SSTidalCorridor_Text_19913B, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_15FDF8:: @ 815FDF8
	setrespawn HEAL_LOCATION_LILYCOVE_CITY
	msgbox SSTidalCorridor_Text_1991F4, MSGBOX_DEFAULT
	call_if_set FLAG_RECEIVED_TM49, SSTidalCorridor_EventScript_15FE36
	warp MAP_LILYCOVE_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

SSTidalCorridor_EventScript_15FE17:: @ 815FE17
	setrespawn HEAL_LOCATION_SLATEPORT_CITY
	msgbox SSTidalCorridor_Text_1991F4, MSGBOX_DEFAULT
	call_if_set FLAG_RECEIVED_TM49, SSTidalCorridor_EventScript_15FE36
	warp MAP_SLATEPORT_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

SSTidalCorridor_EventScript_15FE36:: @ 815FE36
	setflag FLAG_HIDE_TM_49_GIVER_SS_TIDAL
	return

SSTidalCorridor_EventScript_Porthole:: @ 815FE3A
	lockall
	compare VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_SLATEPORT
	goto_if_eq SSTidalCorridor_EventScript_LookThroughPorthole
	compare VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_SLATEPORT
	goto_if_eq SSTidalCorridor_EventScript_LookThroughPorthole
	msgbox SSTidalCorridor_Text_HorizonSpreadsBeyondPorthole, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_LookThroughPorthole:: @ 815FE5B
	special LookThroughPorthole
	waitstate
	end

SSTidalCorridor_EventScript_15FE60:: @ 815FE60
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_SS_TIDAL_TRAINERS, SSTidalCorridor_EventScript_15FE7A
	call SSTidalCorridor_EventScript_15FE84
	msgbox SSTidalCorridor_Text_199203, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_15FE7A:: @ 815FE7A
	msgbox SSTidalCorridor_Text_199255, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_15FE84:: @ 815FE84
	goto_if_not_defeated TRAINER_PHILLIP, SSTidalCorridor_EventScript_15FED5
	goto_if_not_defeated TRAINER_LEONARD, SSTidalCorridor_EventScript_15FED5
	goto_if_not_defeated TRAINER_COLTON, SSTidalCorridor_EventScript_15FED5
	goto_if_not_defeated TRAINER_TUCKER, SSTidalCorridor_EventScript_15FED5
	goto_if_not_defeated TRAINER_THOMAS, SSTidalCorridor_EventScript_15FED5
	goto_if_not_defeated TRAINER_LEA_AND_JED, SSTidalCorridor_EventScript_15FED5
	goto_if_not_defeated TRAINER_GARRET, SSTidalCorridor_EventScript_15FED5
	goto_if_not_defeated TRAINER_ANETTE, SSTidalCorridor_EventScript_15FED5
	setflag FLAG_DEFEATED_SS_TIDAL_TRAINERS
	goto SSTidalCorridor_EventScript_15FE7A
	return

SSTidalCorridor_EventScript_15FED5:: @ 815FED5
	return
