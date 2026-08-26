FallarborTown_ContestLobby_MapScripts:: @ 81539C1
	map_script MAP_SCRIPT_ON_TRANSITION, FallarborTown_ContestLobby_OnTransition
	.byte 0

FallarborTown_ContestLobby_OnTransition:: @ 81539C7
	call LilycoveCity_ContestLobby_EventScript_TryShowContestReporter
	end

FallarborTown_ContestLobby_EventScript_ContestReceptionist:: @ 81539CD
	setvar VAR_0x800B, 6
	setvar VAR_CONTEST_RANK, 1
	call LilycoveCity_ContestLobby_EventScript_SpeakToContestReceptionist
	call FallarborTown_ContestLobby_EventScript_LeadToContestHall
	special SetContestTrainerGfxIds
	call LilycoveCity_ContestLobby_EventScript_SetupRivalGfxId
	setvar VAR_CONTEST_LOCATION, 2
	warp MAP_LINK_CONTEST_ROOM1, 255, 7, 5
	waitstate
	end

FallarborTown_ContestLobby_EventScript_LeadToContestHall:: @ 81539F8
	lockall
	applymovement LOCALID_CONTEST_LOBBY_RECEPTIONIST, FallarborTown_ContestLobby_Movement_ReceptionistApproachCounter
	waitmovement 0
	playse SE_BRIDGE_WALK
	setmetatile 4, 2, METATILE_Contest_WallShadow, TRUE
	setmetatile 4, 3, METATILE_Contest_FloorShadow, TRUE
	special DrawWholeMapView
	applymovement LOCALID_CONTEST_LOBBY_RECEPTIONIST, FallarborTown_ContestLobby_Movement_ReceptionistExitCounter
	waitmovement 0
	playse SE_BRIDGE_WALK
	setmetatile 4, 2, METATILE_Contest_CounterFlap_Top, TRUE
	setmetatile 4, 3, METATILE_Contest_CounterFlap_Bottom, TRUE
	special DrawWholeMapView
	delay 20
	applymovement LOCALID_CONTEST_LOBBY_RECEPTIONIST, FallarborTown_ContestLobby_Movement_ReceptionistFacePlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, FallarborTown_ContestLobby_Movement_PlayerApproachReceptionist
	waitmovement 0
	msgbox LilycoveCity_ContestLobby_Text_ComeThroughHere, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_CONTEST_LOBBY_RECEPTIONIST, FallarborTown_ContestLobby_Movement_ReceptionistWalkToContestHall
	applymovement LOCALID_PLAYER, FallarborTown_ContestLobby_Movement_PlayerWalkToContestHall
	waitmovement 0
	releaseall
	return

FallarborTown_ContestLobby_Movement_PlayerWalkToContestHall:: @ 8153A70
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	delay_8
	set_invisible
	step_end

FallarborTown_ContestLobby_Movement_PlayerApproachReceptionist:: @ 8153A7A
	walk_in_place_fastest_left
	step_end

FallarborTown_ContestLobby_Movement_ReceptionistApproachCounter:: @ 8153A7C
	walk_left
	walk_in_place_fastest_down
	step_end

FallarborTown_ContestLobby_Movement_ReceptionistExitCounter:: @ 8153A7F
	walk_down
	walk_down
	walk_in_place_fastest_up
	step_end

FallarborTown_ContestLobby_Movement_ReceptionistWalkToContestHall:: @ 8153A83
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	delay_8
	set_invisible
	step_end

FallarborTown_ContestLobby_Movement_ReceptionistFacePlayer:: @ 8153A8C
	walk_in_place_fastest_right
	step_end

FallarborTown_ContestLobby_EventScript_Boy:: @ 8153A8E
	msgbox FallarborTown_ContestLobby_Text_UsedToHaveSketch, MSGBOX_NPC
	end

FallarborTown_ContestLobby_EventScript_SketchArtist:: @ 8153A97
	goto_if_set FLAG_CONTEST_SKETCH_CREATED, FallarborTown_ContestLobby_EventScript_SketchArtistShowSketch
	msgbox FallarborTown_ContestLobby_Text_ICreateSketches, MSGBOX_NPC
	end

FallarborTown_ContestLobby_EventScript_SketchArtistShowSketch:: @ 8153AA9
	msgbox FallarborTown_ContestLobby_Text_YourSketchIsUp, MSGBOX_NPC
	end

FallarborTown_ContestLobby_EventScript_Lass:: @ 8153AB2
	msgbox FallarborTown_ContestLobby_Text_AreYouMakingPokeblock, MSGBOX_NPC
	end

FallarborTown_ContestLobby_EventScript_ContestWinner1:: @ 8153ABB
	lockall
	fadescreen FADE_TO_BLACK
	showcontestwinner 2
	releaseall
	end

FallarborTown_ContestLobby_EventScript_ContestWinner2:: @ 8153AC2
	lockall
	special ShowBerryBlenderRecordWindow
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end
