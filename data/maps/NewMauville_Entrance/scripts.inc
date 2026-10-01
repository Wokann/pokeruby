NewMauville_Entrance_MapScripts:: @ 815E48A
	map_script MAP_SCRIPT_ON_LOAD, NewMauville_Entrance_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, NewMauville_Entrance_OnTransition
	.byte 0

NewMauville_Entrance_OnLoad:: @ 815E495
	compare VAR_NEW_MAUVILLE_STATE, 0
	call_if_eq NewMauville_Entrance_EventScript_CloseDoor
	end

NewMauville_Entrance_EventScript_CloseDoor:: @ 815E4A1
	setmetatile 3, 0, METATILE_Facility_NewMauvilleDoor_Closed_Tile0, TRUE
	setmetatile 4, 0, METATILE_Facility_NewMauvilleDoor_Closed_Tile1, TRUE
	setmetatile 5, 0, METATILE_Facility_NewMauvilleDoor_Closed_Tile2, TRUE
	setmetatile 3, 1, METATILE_Facility_NewMauvilleDoor_Closed_Tile3, TRUE
	setmetatile 4, 1, METATILE_Facility_NewMauvilleDoor_Closed_Tile4, TRUE
	setmetatile 5, 1, METATILE_Facility_NewMauvilleDoor_Closed_Tile5, TRUE
	return

NewMauville_Entrance_OnTransition:: @ 815E4D8
	setflag FLAG_LANDMARK_NEW_MAUVILLE
	end

NewMauville_Entrance_EventScript_Door:: @ 815E4DC
	lockall
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox NewMauville_Entrance_Text_DoorIsLocked, MSGBOX_DEFAULT
	checkitem ITEM_BASEMENT_KEY, 1
	compare VAR_RESULT, FALSE
	goto_if_eq NewMauville_Entrance_EventScript_DontOpenDoor
	msgbox NewMauville_Entrance_Text_UseBasementKey, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq NewMauville_Entrance_EventScript_DontOpenDoor
	msgbox NewMauville_Entrance_Text_UsedBasementKey, MSGBOX_DEFAULT
	setmetatile 3, 0, METATILE_Facility_NewMauvilleDoor_Open_Tile0, FALSE
	setmetatile 4, 0, METATILE_Facility_NewMauvilleDoor_Open_Tile1, FALSE
	setmetatile 5, 0, METATILE_Facility_NewMauvilleDoor_Open_Tile2, FALSE
	setmetatile 3, 1, METATILE_Facility_NewMauvilleDoor_Open_Tile3, TRUE
	setmetatile 4, 1, METATILE_Facility_NewMauvilleDoor_Open_Tile4, FALSE
	setmetatile 5, 1, METATILE_Facility_NewMauvilleDoor_Open_Tile5, TRUE
	special DrawWholeMapView
	playse SE_BANG
	setvar VAR_NEW_MAUVILLE_STATE, 1
	releaseall
	end

NewMauville_Entrance_EventScript_DontOpenDoor:: @ 815E55D
	releaseall
	end
