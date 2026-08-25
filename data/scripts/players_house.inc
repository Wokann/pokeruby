PlayersHouse_2F_EventScript_BlockStairsUntilClockIsSet:: @ 81B6950
	setvar VAR_LITTLEROOT_INTRO_STATE, 5
	return

PlayersHouse_1F_EventScript_EnterHouseMovingIn:: @ 81B6956
	msgbox PlayersHouse_1F_Text_IsntItNiceInHere, MSGBOX_DEFAULT
	applymovement VAR_0x8004, Common_Movement_FacePlayer
	waitmovement 0
	compare VAR_0x8005, 0
	call_if_eq PlayersHouse_1F_EventScript_MomFacePlayerMovingInMale
	compare VAR_0x8005, 1
	call_if_eq PlayersHouse_1F_EventScript_MomFacePlayerMovingInFemale
	msgbox PlayersHouse_1F_Text_MoversPokemonGoSetClock, MSGBOX_DEFAULT
	closemessage
	setvar VAR_LITTLEROOT_INTRO_STATE, 4
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerWalkIn
	applymovement VAR_0x8004, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	releaseall
	end

PlayersHouse_1F_EventScript_MomFacePlayerMovingInMale:: @ 81B699F
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	return

PlayersHouse_1F_EventScript_MomFacePlayerMovingInFemale:: @ 81B69AA
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	return

PlayersHouse_1F_Movement_PlayerWalkIn:: @ 81B69B5
	walk_up
	step_end

PlayersHouse_1F_EventScript_MomGoSeeRoom:: @ 81B69B7
	msgbox PlayersHouse_1F_Text_ArentYouInterestedInRoom, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_0x8004, Common_Movement_WalkInPlaceFastestUp
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_MovePlayerAwayFromDoor
	waitmovement 0
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_WallClock:: @ 81B69D3
	lockall
	setvar VAR_0x8004, 0
	goto PlayersHouse_2F_EventScript_WallClock
	end

LittlerootTown_MaysHouse_2F_EventScript_WallClock:: @ 81B69DF
	lockall
	setvar VAR_0x8004, 1
	goto PlayersHouse_2F_EventScript_WallClock
	end

PlayersHouse_2F_EventScript_WallClock:: @ 81B69EB
	goto_if_set FLAG_SET_WALL_CLOCK, PlayersHouse_2F_EventScript_CheckWallClock
	msgbox PlayersHouse_2F_Text_ClockIsStopped, MSGBOX_DEFAULT
	call PlayersHouse_2F_EventScript_SetWallClock
	delay 30
	setvar VAR_LITTLEROOT_INTRO_STATE, 6
	setflag FLAG_SET_WALL_CLOCK
	setflag FLAG_HIDE_MACHOKE_MOVER_1
	setflag FLAG_HIDE_MACHOKE_MOVER_2
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq PlayersHouse_2F_EventScript_MomComesUpstairsMale
	compare VAR_RESULT, FEMALE
	call_if_eq PlayersHouse_2F_EventScript_MomComesUpstairsFemale
	playse SE_EXIT
	removeobject VAR_0x8008
	releaseall
	end

PlayersHouse_2F_EventScript_MomComesUpstairsMale:: @ 81B6A31
	setvar VAR_0x8008, LOCALID_PLAYERS_HOUSE_2F_MOM
	addobject VAR_0x8008
	applymovement VAR_0x8008, PlayersHouse_2F_Movement_MomEntersMale
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	msgbox PlayersHouse_2F_Text_HowDoYouLikeYourRoom, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_0x8008, PlayersHouse_2F_Movement_MomExitsMale
	waitmovement 0
	return

PlayersHouse_2F_EventScript_MomComesUpstairsFemale:: @ 81B6A61
	setvar VAR_0x8008, LOCALID_PLAYERS_HOUSE_2F_MOM
	addobject VAR_0x8008
	applymovement VAR_0x8008, PlayersHouse_2F_Movement_MomEntersFemale
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	msgbox PlayersHouse_2F_Text_HowDoYouLikeYourRoom, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_0x8008, PlayersHouse_2F_Movement_MomExitsFemale
	waitmovement 0
	return

PlayersHouse_2F_EventScript_CheckWallClock:: @ 81B6A91
	incrementgamestat GAME_STAT_CHECKED_CLOCK
	fadescreen FADE_TO_BLACK
	special ScrSpecial_ViewWallClock
	waitstate
	releaseall
	end

PlayersHouse_2F_EventScript_SetWallClock:: @ 81B6A9B
	fadescreen FADE_TO_BLACK
	special StartWallClock
	waitstate
	return

PlayersHouse_2F_Movement_MomEntersMale:: @ 81B6AA2
	delay_8
	walk_down
	walk_in_place_fastest_left
	delay_16
	delay_8
	walk_left
	step_end

PlayersHouse_2F_Movement_MomExitsMale:: @ 81B6AA9
	walk_right
	walk_up
	delay_8
	step_end

PlayersHouse_2F_Movement_MomEntersFemale:: @ 81B6AAD
	delay_8
	walk_down
	walk_in_place_fastest_right
	delay_16
	delay_8
	walk_right
	step_end

PlayersHouse_2F_Movement_MomExitsFemale:: @ 81B6AB4
	walk_left
	walk_up
	delay_8
	step_end

PlayersHouse_1F_EventScript_SetWatchedBroadcast:: @ 81B6AB8
	setvar VAR_LITTLEROOT_INTRO_STATE, 7
	releaseall
	end

PlayersHouse_1F_EventScript_PetalburgGymReportMale:: @ 81B6ABF
	applymovement VAR_0x8005, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	call PlayersHouse_1F_EventScript_MomNoticeGymBroadcast
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerApproachTVForGymMale
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTERVIEWER, FALSE
	msgbox PlayersHouse_1F_Text_MaybeDadWillBeOn, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_0x8005, PlayersHouse_1F_Movement_MomMakeRoomToSeeTVMale
	waitmovement 0
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerMoveToTVMale
	waitmovement 0
	call PlayersHouse_1F_EventScript_WatchGymBroadcast
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	msgbox PlayersHouse_1F_Text_ItsOverWeMissedHim, MSGBOX_DEFAULT
	msgbox PlayersHouse_1F_Text_GoIntroduceYourselfNextDoor, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_1, 1
	applymovement VAR_0x8005, PlayersHouse_1F_Movement_MomReturnToSeatMale
	waitmovement 0
	goto PlayersHouse_1F_EventScript_SetWatchedBroadcast
	end

PlayersHouse_1F_EventScript_PetalburgGymReportFemale:: @ 81B6B2E
	applymovement VAR_0x8005, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	call PlayersHouse_1F_EventScript_MomNoticeGymBroadcast
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerApproachTVForGymFemale
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTERVIEWER, FALSE
	msgbox PlayersHouse_1F_Text_MaybeDadWillBeOn, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_0x8005, PlayersHouse_1F_Movement_MomMakeRoomToSeeTVFemale
	waitmovement 0
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerMoveToTVFemale
	waitmovement 0
	call PlayersHouse_1F_EventScript_WatchGymBroadcast
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	msgbox PlayersHouse_1F_Text_ItsOverWeMissedHim, MSGBOX_DEFAULT
	msgbox PlayersHouse_1F_Text_GoIntroduceYourselfNextDoor, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_1, 1
	applymovement VAR_0x8005, PlayersHouse_1F_Movement_MomReturnToSeatFemale
	waitmovement 0
	goto PlayersHouse_1F_EventScript_SetWatchedBroadcast
	end

PlayersHouse_1F_EventScript_MomNoticeGymBroadcast:: @ 81B6B9D
	playse SE_PIN
	applymovement VAR_0x8005, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_0x8005, Common_Movement_Delay48
	waitmovement 0
	msgbox PlayersHouse_1F_Text_OhComeQuickly, MSGBOX_DEFAULT
	closemessage
	return

PlayersHouse_1F_EventScript_WatchGymBroadcast:: @ 81B6BBE
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	msgbox PlayersHouse_1F_Text_ReportFromPetalburgGym, MSGBOX_DEFAULT
	fadedefaultbgm
	special TurnOffTVScreen
	setflag FLAG_SYS_TV_HOME
	delay 35
	return

PlayersHouse_1F_Movement_MomMakeRoomToSeeTVMale:: @ 81B6BDB
	walk_left
	walk_in_place_fastest_right
	step_end

PlayersHouse_1F_Movement_MomMakeRoomToSeeTVFemale:: @ 81B6BDE
	walk_right
	walk_in_place_fastest_left
	step_end

PlayersHouse_1F_Movement_MomReturnToSeatMale:: @ 81B6BE1
	walk_left
	walk_down
	walk_in_place_fastest_right
	step_end

PlayersHouse_1F_Movement_MomReturnToSeatFemale:: @ 81B6BE5
	walk_right
	walk_down
	walk_in_place_fastest_left
	step_end

PlayersHouse_1F_EventScript_Mom:: @ 81B6BE9
	lock
	faceplayer
	compare VAR_LITTLEROOT_HOUSES_STATE, 4
	goto_if_eq PlayersHouse_1F_EventScript_DadShouldStayLonger
	compare VAR_LITTLEROOT_HOUSES_STATE_2, 4
	goto_if_eq PlayersHouse_1F_EventScript_DadShouldStayLonger
	goto_if_set FLAG_BADGE05_GET, PlayersHouse_1F_EventScript_TryGiveAmuletCoin
	goto_if_set FLAG_RESCUED_BIRCH, PlayersHouse_1F_EventScript_MomHealsParty
	compare VAR_TEMP_1, 1
	goto_if_eq PlayersHouse_1F_EventScript_SeeYouHoney
	compare VAR_LITTLEROOT_INTRO_STATE, 7
	goto_if_ge PlayersHouse_1F_EventScript_DidYouMeetProfBirch
	msgbox PlayersHouse_1F_Text_IsntItNiceInHere, MSGBOX_DEFAULT
	release
	end

PlayersHouse_1F_EventScript_DadShouldStayLonger:: @ 81B6C33
	msgbox PlayersHouse_1F_Text_DadShouldStayLonger, MSGBOX_DEFAULT
	release
	end

PlayersHouse_1F_EventScript_TryGiveAmuletCoin:: @ 81B6C3D
	goto_if_set FLAG_RECEIVED_AMULET_COIN, PlayersHouse_1F_EventScript_MomHealsParty
	msgbox PlayersHouse_1F_Text_GotDadsBadgeHeresSomethingFromMom, MSGBOX_DEFAULT
	giveitem ITEM_AMULET_COIN
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	msgbox PlayersHouse_1F_Text_DontPushYourselfTooHard, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_AMULET_COIN
	release
	end

PlayersHouse_1F_EventScript_MomHealsParty:: @ 81B6C72
	msgbox PlayersHouse_1F_Text_YouShouldRestABit, MSGBOX_DEFAULT
	goto PlayersHouse_1F_EventScript_HealParty
	end

PlayersHouse_1F_EventScript_SeeYouHoney:: @ 81B6C80
	msgbox PlayersHouse_1F_Text_SeeYouHoney, MSGBOX_DEFAULT
	release
	end

PlayersHouse_1F_EventScript_HealParty:: @ 81B6C8A
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	incrementgamestat GAME_STAT_RESTED_AT_HOME
	msgbox PlayersHouse_1F_Text_TakeCareHoney, MSGBOX_DEFAULT
	release
	end

PlayersHouse_1F_EventScript_DidYouMeetProfBirch:: @ 81B6C9C
	msgbox PlayersHouse_1F_Text_DidYouMeetProfBirch, MSGBOX_DEFAULT
	release
	end

PlayersHouse_1F_EventScript_Machoke1:: @ 81B6CA6
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MACHOKE, 0
	msgbox PlayersHouse_1F_Text_Machoke1, MSGBOX_DEFAULT
	waitmoncry
	release
	end

PlayersHouse_1F_EventScript_Machoke2:: @ 81B6CB9
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MACHOKE, 0
	msgbox PlayersHouse_1F_Text_Machoke2, MSGBOX_DEFAULT
	waitmoncry
	release
	end

PlayersHouse_1F_Movement_PlayerApproachTVForGymMale:: @ 81B6CCC
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	step_end

PlayersHouse_1F_Movement_PlayerMoveToTVMale:: @ 81B6CD2
	walk_left
	step_end

PlayersHouse_1F_Movement_PlayerApproachTVForGymFemale:: @ 81B6CD4
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	step_end

PlayersHouse_1F_Movement_PlayerMoveToTVFemale:: @ 81B6CDA
	walk_right
	step_end

PlayersHouse_1F_Movement_MovePlayerAwayFromDoor:: @ 81B6CDC
	walk_up
	step_end

PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV:: @ 81B6CDE
	lockall
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq PlayersHouse_1F_EventScript_SetUpObjectEventVarsMale
	compare VAR_RESULT, FEMALE
	call_if_eq PlayersHouse_1F_EventScript_SetUpObjectEventVarsFemale
	compare VAR_0x8008, MALE
	call_if_eq PlayersHouse_1F_EventScript_PlayerEnterRoomMale
	compare VAR_0x8008, FEMALE
	call_if_eq PlayersHouse_1F_EventScript_PlayerEnterRoomFemale
	applymovement VAR_0x8009, Common_Movement_FacePlayer
	waitmovement 0
	playse SE_PIN
	applymovement VAR_0x8009, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_0x8009, Common_Movement_Delay48
	waitmovement 0
	delay 20
	compare VAR_0x8008, MALE
	call_if_eq PlayersHouse_1F_EventScript_DadApproachPlayerMale
	compare VAR_0x8008, FEMALE
	call_if_eq PlayersHouse_1F_EventScript_DadApproachPlayerFemale
	msgbox PlayersHouse_1F_Text_TicketFromBrineyCameForYou, MSGBOX_DEFAULT
	giveitem ITEM_SS_TICKET
	msgbox PlayersHouse_1F_Text_PortsInSlateportLilycove, MSGBOX_DEFAULT
	closemessage
	compare VAR_0x8008, MALE
	call_if_eq PlayersHouse_1F_EventScript_DadApproachMomMale
	compare VAR_0x8008, FEMALE
	call_if_eq PlayersHouse_1F_EventScript_DadApproachMomFemale
	msgbox PlayersHouse_1F_Text_BetterGetBackToGym, MSGBOX_DEFAULT
	closemessage
	compare VAR_0x8008, MALE
	call_if_eq PlayersHouse_1F_EventScript_DadExitsMale
	compare VAR_0x8008, FEMALE
	call_if_eq PlayersHouse_1F_EventScript_DadExitsFemale
	playse SE_DOOR
	removeobject VAR_0x8009
	setflag FLAG_RECEIVED_SS_TICKET
	setvar VAR_LITTLEROOT_HOUSES_STATE, 4
	setvar VAR_LITTLEROOT_HOUSES_STATE_2, 4
	releaseall
	end

PlayersHouse_1F_EventScript_SetUpObjectEventVarsMale:: @ 81B6DAD
	setvar VAR_0x8008, MALE
	setvar VAR_0x8009, LOCALID_PLAYERS_HOUSE_1F_DAD
	setvar VAR_0x800A, LOCALID_PLAYERS_HOUSE_1F_MOM
	return

PlayersHouse_1F_EventScript_SetUpObjectEventVarsFemale:: @ 81B6DBD
	setvar VAR_0x8008, FEMALE
	setvar VAR_0x8009, LOCALID_PLAYERS_HOUSE_1F_DAD
	setvar VAR_0x800A, LOCALID_PLAYERS_HOUSE_1F_MOM
	return

PlayersHouse_1F_EventScript_DadApproachPlayerMale:: @ 81B6DCD
	applymovement VAR_0x8009, PlayersHouse_1F_Movement_DadApproachPlayerMale
	waitmovement 0
	return

PlayersHouse_1F_EventScript_DadApproachPlayerFemale:: @ 81B6DD8
	applymovement VAR_0x8009, PlayersHouse_1F_Movement_DadApproachPlayerFemale
	waitmovement 0
	return

PlayersHouse_1F_EventScript_DadApproachMomMale:: @ 81B6DE3
	applymovement VAR_0x8009, PlayersHouse_1F_Movement_DadApproachMomMale
	waitmovement 0
	return

PlayersHouse_1F_EventScript_DadApproachMomFemale:: @ 81B6DEE
	applymovement VAR_0x8009, PlayersHouse_1F_Movement_DadApproachMomFemale
	waitmovement 0
	return

PlayersHouse_1F_EventScript_DadExitsMale:: @ 81B6DF9
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerWatchDadExit
	applymovement VAR_0x8009, PlayersHouse_1F_Movement_DadExitsMale
	waitmovement 0
	return

PlayersHouse_1F_EventScript_DadExitsFemale:: @ 81B6E0B
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerWatchDadExit
	applymovement VAR_0x8009, PlayersHouse_1F_Movement_DadExitsFemale
	waitmovement 0
	return

PlayersHouse_1F_EventScript_PlayerEnterRoomMale:: @ 81B6E1D
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerEnterRoomMale
	waitmovement 0
	return

PlayersHouse_1F_EventScript_PlayerEnterRoomFemale:: @ 81B6E28
	applymovement LOCALID_PLAYER, PlayersHouse_1F_Movement_PlayerEnterRoomFemale
	waitmovement 0
	return

PlayersHouse_1F_Movement_DadApproachPlayerMale:: @ 81B6E33
	walk_right
	walk_right
	step_end

PlayersHouse_1F_Movement_DadApproachPlayerFemale:: @ 81B6E36
	walk_left
	walk_left
	step_end

PlayersHouse_1F_Movement_DadApproachMomMale:: @ 81B6E39
	walk_left
	step_end

PlayersHouse_1F_Movement_DadApproachMomFemale:: @ 81B6E3B
	walk_right
	step_end

PlayersHouse_1F_Movement_DadExitsMale:: @ 81B6E3D
	walk_down
	walk_right
	walk_right
	walk_down
	delay_8
	step_end

PlayersHouse_1F_Movement_DadExitsFemale:: @ 81B6E43
	walk_down
	walk_left
	walk_left
	walk_down
	delay_8
	step_end

PlayersHouse_1F_Movement_PlayerEnterRoomMale:: @ 81B6E49
	delay_16
	walk_down
	walk_down
	walk_down
	walk_in_place_fastest_left
	step_end

PlayersHouse_1F_Movement_PlayerWatchDadExit:: @ 81B6E4F
	delay_16
	delay_16
	delay_16
	walk_in_place_fastest_down
	step_end

PlayersHouse_1F_Movement_PlayerEnterRoomFemale:: @ 81B6E54
	delay_16
	walk_down
	walk_down
	walk_down
	walk_in_place_fastest_right
	step_end
