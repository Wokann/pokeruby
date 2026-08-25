OldaleTown_MapScripts:: @ 814DD62
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_OnTransition
	.byte 0

OldaleTown_OnTransition:: @ 814DD68
	call Common_EventScript_SetupRivalGfxId
	setflag FLAG_VISITED_OLDALE_TOWN
	call_if_unset FLAG_ADVENTURE_STARTED, OldaleTown_EventScript_BlockWestEntrance
	call_if_unset FLAG_RECEIVED_POTION_OLDALE, OldaleTown_EventScript_MoveMartEmployee
	call_if_set FLAG_ADVENTURE_STARTED, OldaleTown_EventScript_SetOldaleState
	end

OldaleTown_EventScript_SetOldaleState:: @ 814DD8C
	setvar VAR_OLDALE_TOWN_STATE, 1
	return

OldaleTown_EventScript_BlockWestEntrance:: @ 814DD92
	setobjectxyperm LOCALID_FOOTPRINTS_MAN, 1, 11
	setobjectmovementtype LOCALID_FOOTPRINTS_MAN, 9
	return

OldaleTown_EventScript_MoveMartEmployee:: @ 814DD9E
	setobjectxyperm LOCALID_OLDALE_MART_EMPLOYEE, 13, 14
	setobjectmovementtype LOCALID_OLDALE_MART_EMPLOYEE, 8
	return

OldaleTown_EventScript_TownSign:: @ 814DDAA
	msgbox OldaleTown_Text_TownSign, MSGBOX_SIGN
	end

OldaleTown_EventScript_Girl:: @ 814DDB3
	msgbox OldaleTown_Text_SavingMyProgress, MSGBOX_NPC
	end

OldaleTown_EventScript_MartEmployee:: @ 814DDBC
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_POTION_OLDALE, OldaleTown_EventScript_ExplainPotion
	goto_if_set FLAG_TEMP_1, OldaleTown_EventScript_ExplainPotion
	setflag FLAG_TEMP_1
	playbgm MUS_FOLLOW_ME, FALSE
	msgbox OldaleTown_Text_IWorkAtPokemonMart, MSGBOX_DEFAULT
	closemessage
	switch VAR_FACING
	case DIR_SOUTH, OldaleTown_EventScript_GoToMartSouth
	case DIR_NORTH, OldaleTown_EventScript_GoToMartNorth
	case DIR_EAST, OldaleTown_EventScript_GoToMartEast
	end

OldaleTown_EventScript_GoToMartSouth:: @ 814DE07
	applymovement LOCALID_OLDALE_MART_EMPLOYEE, OldaleTown_Movement_EmployeeSouth
	applymovement LOCALID_PLAYER, OldaleTown_Movement_PlayerSouth
	waitmovement 0
	goto OldaleTown_EventScript_ExplainPokemonMart
	end

OldaleTown_EventScript_GoToMartNorth:: @ 814DE1E
	applymovement LOCALID_OLDALE_MART_EMPLOYEE, OldaleTown_Movement_EmployeeNorth
	applymovement LOCALID_PLAYER, OldaleTown_Movement_PlayerNorth
	waitmovement 0
	goto OldaleTown_EventScript_ExplainPokemonMart
	end

OldaleTown_EventScript_GoToMartEast:: @ 814DE35
	applymovement LOCALID_PLAYER, OldaleTown_Movement_PlayerEast
	applymovement LOCALID_OLDALE_MART_EMPLOYEE, OldaleTown_Movement_EmployeeEast
	waitmovement 0
	goto OldaleTown_EventScript_ExplainPokemonMart
	end

OldaleTown_EventScript_ExplainPokemonMart:: @ 814DE4C
	msgbox OldaleTown_Text_ThisIsAPokemonMart, MSGBOX_DEFAULT
	giveitem ITEM_POTION
	compare VAR_RESULT, 0
	goto_if_eq OldaleTown_EventScript_BagIsFull
	msgbox OldaleTown_Text_PotionExplanation, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_POTION_OLDALE
	fadedefaultbgm
	release
	end

OldaleTown_EventScript_ExplainPotion:: @ 814DE79
	msgbox OldaleTown_Text_PotionExplanation, MSGBOX_DEFAULT
	release
	end

OldaleTown_EventScript_BagIsFull:: @ 814DE83
	msgbox Text_TooBadBagIsFull, MSGBOX_DEFAULT
	fadedefaultbgm
	release
	end

OldaleTown_Movement_EmployeeEast:: @ 814DE8E
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_in_place_fastest_down
	step_end

OldaleTown_Movement_EmployeeSouth:: @ 814DE97
	walk_left
	walk_up
	walk_up
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_in_place_fastest_down
	step_end

OldaleTown_Movement_EmployeeNorth:: @ 814DEA2
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_in_place_fastest_down
	step_end

OldaleTown_Movement_Unused1:: @ 814DEAB
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	delay_8
	walk_in_place_fastest_down
	step_end

OldaleTown_Movement_PlayerEast:: @ 814DEB9
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

OldaleTown_Movement_PlayerSouth:: @ 814DEC1
	delay_16
	delay_16
	delay_16
	delay_16
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

OldaleTown_Movement_PlayerNorth:: @ 814DECB
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

OldaleTown_Movement_Unused2:: @ 814DED3
	walk_left
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

OldaleTown_EventScript_FootprintsMan:: @ 814DEDF
	lock
	faceplayer
	goto_if_set FLAG_ADVENTURE_STARTED, OldaleTown_EventScript_NotBlockingPath
	msgbox OldaleTown_Text_DiscoveredFootprints, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_FOOTPRINTS_MAN, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

OldaleTown_EventScript_BlockedPath:: @ 814DEFF
	lockall
	applymovement LOCALID_PLAYER, OldaleTown_Movement_PlayerStepBack
	applymovement LOCALID_FOOTPRINTS_MAN, OldaleTown_Movement_BackUp
	waitmovement 0
	msgbox OldaleTown_Text_WaitDontComeInHere, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_FOOTPRINTS_MAN, OldaleTown_Movement_ReturnToOriginalPosition
	waitmovement 0
	releaseall
	end

OldaleTown_EventScript_NotBlockingPath:: @ 814DF26
	msgbox OldaleTown_Text_FinishedSketchingFootprints, MSGBOX_DEFAULT
	release
	end

OldaleTown_EventScript_Rival:: @ 814DF30
	lockall
	applymovement LOCALID_OLDALE_RIVAL, Common_Movement_FacePlayer
	waitmovement 0
	goto OldaleTown_EventScript_ShowRivalMessage
	end

OldaleTown_EventScript_RivalTrigger1:: @ 814DF41
	lockall
	applymovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalApproachPlayer1
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	goto OldaleTown_EventScript_ShowRivalMessage
	end

OldaleTown_EventScript_RivalTrigger2:: @ 814DF5C
	lockall
	applymovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalApproachPlayer2
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	goto OldaleTown_EventScript_ShowRivalMessage
	end

OldaleTown_EventScript_RivalTrigger3:: @ 814DF77
	lockall
	applymovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalApproachPlayer3
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	goto OldaleTown_EventScript_ShowRivalMessage
	end

OldaleTown_EventScript_ShowRivalMessage:: @ 814DF92
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq OldaleTown_EventScript_ShowMayMessage
	compare VAR_RESULT, FEMALE
	goto_if_eq OldaleTown_EventScript_ShowBrendanMessage
	end

OldaleTown_EventScript_ShowMayMessage:: @ 814DFAA
	msgbox OldaleTown_Text_MayLetsGoBack, MSGBOX_DEFAULT
	goto OldaleTown_EventScript_RivalFinish
	end

OldaleTown_EventScript_ShowBrendanMessage:: @ 814DFB8
	msgbox OldaleTown_Text_BrendanLetsGoBack, MSGBOX_DEFAULT
	goto OldaleTown_EventScript_RivalFinish
	end

OldaleTown_EventScript_RivalFinish:: @ 814DFC6
	closemessage
	applymovement LOCALID_OLDALE_RIVAL, OldaleTown_Movement_RivalExit
	waitmovement 0
	removeobject LOCALID_OLDALE_RIVAL
	setvar VAR_OLDALE_RIVAL_STATE, 2
	setflag FLAG_HIDE_OLDALE_TOWN_RIVAL
	releaseall
	end

OldaleTown_Movement_RivalApproachPlayer1:: @ 814DFDE
	walk_left
	walk_left
	step_end

OldaleTown_Movement_RivalApproachPlayer2:: @ 814DFE1
	walk_left
	step_end

OldaleTown_Movement_RivalApproachPlayer3:: @ 814DFE3
	face_left
	step_end

OldaleTown_Movement_RivalExit:: @ 814DFE5
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

OldaleTown_Movement_PlayerStepBack:: @ 814DFEC
	delay_8
	walk_right
	step_end

OldaleTown_Movement_BackUp:: @ 814DFEF
	walk_fast_up
	walk_in_place_fastest_left
	lock_facing_direction
	walk_right
	unlock_facing_direction
	step_end

OldaleTown_Movement_ReturnToOriginalPosition:: @ 814DFF5
	walk_down
	walk_left
	step_end
