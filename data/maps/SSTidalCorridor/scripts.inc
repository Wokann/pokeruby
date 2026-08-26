SSTidalCorridor_MapScripts:: @ 815FC94
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SSTidalCorridor_OnFrame
	.byte 0

SSTidalCorridor_OnFrame:: @ 815FC9A
	map_script_2 VAR_PORTHOLE_STATE, 1, SSTidalCorridor_EventScript_DepartSlateportForLilycove
	map_script_2 VAR_PORTHOLE_STATE, 5, SSTidalCorridor_EventScript_DepartLilycoveForSlateport
	map_script_2 VAR_PORTHOLE_STATE, 9, SSTidalCorridor_EventScript_HalfwayToLilycove
	map_script_2 VAR_PORTHOLE_STATE, 10, SSTidalCorridor_EventScript_ArrivedInSlateport
	.2byte 0

SSTidalCorridor_EventScript_DepartSlateportForLilycove:: @ 815FCBC
	special SetSSTidalFlag
	setvar VAR_PORTHOLE_STATE, 2
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_FastCurrentsHopeYouEnjoyVoyage, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_DepartLilycoveForSlateport:: @ 815FCD2
	setvar VAR_PORTHOLE_STATE, 6
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_HopeYouEnjoyVoyage, MSGBOX_DEFAULT
	releaseall
	end

SSTidalRooms_EventScript_HalfwayToSlateport:: @ 815FCE5
	special SetSSTidalFlag
	setvar VAR_PORTHOLE_STATE, 7
	playse SE_DING_DONG
	msgbox SSTidal_Text_FastCurrentsHopeYouEnjoyVoyage, MSGBOX_DEFAULT
	return

SSTidalRooms_EventScript_ArrivedInLilycove:: @ 815FCF9
	special ResetSSTidalFlag
	setvar VAR_PORTHOLE_STATE, 4
	playse SE_DING_DONG
	msgbox SSTidal_Text_MadeLandInLilycove, MSGBOX_DEFAULT
	return

SSTidalCorridor_EventScript_ReachedStepCount:: @ 815FD0D
	compare VAR_PORTHOLE_STATE, 2
	goto_if_eq SSTidalCorridor_EventScript_HalfwayToLilycove
	compare VAR_PORTHOLE_STATE, 7
	goto_if_eq SSTidalCorridor_EventScript_ArrivedInSlateport
	end

SSTidalCorridor_EventScript_HalfwayToLilycove:: @ 815FD24
	special ResetSSTidalFlag
	setvar VAR_PORTHOLE_STATE, 3
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_HopeYouEnjoyVoyage, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_ArrivedInSlateport:: @ 815FD3A
	special ResetSSTidalFlag
	setvar VAR_PORTHOLE_STATE, 8
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_MadeLandInSlateport, MSGBOX_DEFAULT
	releaseall
	end

SSTidalRooms_EventScript_ArrivedInSlateport:: @ 815FD50
	special ResetSSTidalFlag
	setvar VAR_PORTHOLE_STATE, 8
	playse SE_DING_DONG
	msgbox SSTidal_Text_MadeLandInSlateport, MSGBOX_DEFAULT
	return

SSTidalRooms_EventScript_ProgressCruiseAfterBed:: @ 815FD64
	switch VAR_PORTHOLE_STATE
	case 2, SSTidalRooms_EventScript_ArrivedInLilycove
	case 3, SSTidalRooms_EventScript_ArrivedInLilycove
	case 6, SSTidalRooms_EventScript_HalfwayToSlateport
	case 7, SSTidalRooms_EventScript_ArrivedInSlateport
	return

SSTidalCorridor_EventScript_Briney:: @ 815FD96
	msgbox SSTidalCorridor_Text_BrineyWelcomeAboard, MSGBOX_NPC
	end

SSTidalCorridor_EventScript_Peeko:: @ 815FD9F
	lock
	faceplayer
	waitse
	playmoncry SPECIES_WINGULL, 0
	msgbox SSTidalCorridor_Text_Peeko, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SSTidalCorridor_EventScript_Cabin1Sign:: @ 815FDB2
	msgbox SSTidalCorridor_Text_Cabin1, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_Cabin2Sign:: @ 815FDBB
	msgbox SSTidalCorridor_Text_Cabin2, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_Cabin3Sign:: @ 815FDC4
	msgbox SSTidalCorridor_Text_Cabin3, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_Cabin4Sign:: @ 815FDCD
	msgbox SSTidalCorridor_Text_Cabin4, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_ExitSailor:: @ 815FDD6
	lock
	faceplayer
	compare VAR_PORTHOLE_STATE, 4
	goto_if_eq SSTidalCorridor_EventScript_ExitLilycove
	compare VAR_PORTHOLE_STATE, 8
	goto_if_eq SSTidalCorridor_EventScript_ExitSlateport
	msgbox SSTidalCorridor_Text_CanRestInCabin2, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_ExitLilycove:: @ 815FDF8
	setrespawn HEAL_LOCATION_LILYCOVE_CITY
	msgbox SSTidalCorridor_Text_WeveArrived, MSGBOX_DEFAULT
	call_if_set FLAG_RECEIVED_TM49, SSTidalCorridor_EventScript_HideSnatchGiver
	warp MAP_LILYCOVE_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

SSTidalCorridor_EventScript_ExitSlateport:: @ 815FE17
	setrespawn HEAL_LOCATION_SLATEPORT_CITY
	msgbox SSTidalCorridor_Text_WeveArrived, MSGBOX_DEFAULT
	call_if_set FLAG_RECEIVED_TM49, SSTidalCorridor_EventScript_HideSnatchGiver
	warp MAP_SLATEPORT_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

SSTidalCorridor_EventScript_HideSnatchGiver:: @ 815FE36
	setflag FLAG_HIDE_TM_49_GIVER_SS_TIDAL
	return

SSTidalCorridor_EventScript_Porthole:: @ 815FE3A
	lockall
	compare VAR_PORTHOLE_STATE, 2
	goto_if_eq SSTidalCorridor_EventScript_LookThroughPorthole
	compare VAR_PORTHOLE_STATE, 7
	goto_if_eq SSTidalCorridor_EventScript_LookThroughPorthole
	msgbox SSTidalCorridor_Text_HorizonSpreadsBeyondPorthole, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_LookThroughPorthole:: @ 815FE5B
	special sub_80C7958
	waitstate
	end

SSTidalCorridor_EventScript_Sailor:: @ 815FE60
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_SS_TIDAL_TRAINERS, SSTidalCorridor_EventScript_EnjoyYourCruise
	call SSTidalCorridor_EventScript_CheckIfTrainersDefeated
	msgbox SSTidalCorridor_Text_VisitOtherCabins, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_EnjoyYourCruise:: @ 815FE7A
	msgbox SSTidalCorridor_Text_EnjoyYourCruise, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_CheckIfTrainersDefeated:: @ 815FE84
	goto_if_not_defeated TRAINER_PHILLIP, SSTidalCorridor_EventScript_TrainerNotDefeated
	goto_if_not_defeated TRAINER_LEONARD, SSTidalCorridor_EventScript_TrainerNotDefeated
	goto_if_not_defeated TRAINER_COLTON, SSTidalCorridor_EventScript_TrainerNotDefeated
	goto_if_not_defeated TRAINER_TUCKER, SSTidalCorridor_EventScript_TrainerNotDefeated
	goto_if_not_defeated TRAINER_THOMAS, SSTidalCorridor_EventScript_TrainerNotDefeated
	goto_if_not_defeated TRAINER_LEA_AND_JED, SSTidalCorridor_EventScript_TrainerNotDefeated
	goto_if_not_defeated TRAINER_GARRET, SSTidalCorridor_EventScript_TrainerNotDefeated
	goto_if_not_defeated TRAINER_ANETTE, SSTidalCorridor_EventScript_TrainerNotDefeated
	setflag FLAG_DEFEATED_SS_TIDAL_TRAINERS
	goto SSTidalCorridor_EventScript_EnjoyYourCruise
	return

SSTidalCorridor_EventScript_TrainerNotDefeated:: @ 815FED5
	return
