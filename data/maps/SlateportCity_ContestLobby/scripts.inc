SlateportCity_ContestLobby_MapScripts:: @ 8155411
	map_script MAP_SCRIPT_ON_TRANSITION, SlateportCity_ContestLobby_OnTransition
	.byte 0

SlateportCity_ContestLobby_OnTransition:: @ 8155417
	call LilycoveCity_ContestLobby_EventScript_TryShowContestReporter
	end

SlateportCity_ContestLobby_EventScript_ContestReceptionist:: @ 815541D
	setvar VAR_0x800B, 7
	setvar VAR_CONTEST_RANK, 2
	call LilycoveCity_ContestLobby_EventScript_SpeakToContestReceptionist
	call SlateportCity_ContestLobby_EventScript_EnterContestRoom
	special SetContestTrainerGfxIds
	call LilycoveCity_ContestLobby_EventScript_SetupRivalGfxId
	setvar VAR_CONTEST_LOCATION, 3
	warp MAP_LINK_CONTEST_ROOM1, 255, 7, 5
	waitstate
	end

SlateportCity_ContestLobby_EventScript_EnterContestRoom:: @ 8155448
	lockall
	applymovement LOCALID_SLATEPORT_CONTEST_LOBBY_RECEPTIONIST, SlateportCity_ContestLobby_Movement_ReceptionistApproachCounter
	waitmovement 0
	playse SE_BRIDGE_WALK
	setmetatile 4, 2, METATILE_Contest_WallShadow, 1
	setmetatile 4, 3, METATILE_Contest_FloorShadow, 1
	special DrawWholeMapView
	applymovement LOCALID_SLATEPORT_CONTEST_LOBBY_RECEPTIONIST, SlateportCity_ContestLobby_Movement_ReceptionistOpenCounter
	waitmovement 0
	playse SE_BRIDGE_WALK
	setmetatile 4, 2, METATILE_Contest_CounterFlap_Top, 1
	setmetatile 4, 3, METATILE_Contest_CounterFlap_Bottom, 1
	special DrawWholeMapView
	delay 20
	applymovement LOCALID_SLATEPORT_CONTEST_LOBBY_RECEPTIONIST, SlateportCity_ContestLobby_Movement_ReceptionistFacePlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, SlateportCity_ContestLobby_Movement_PlayerFaceReceptionist
	waitmovement 0
	msgbox LilycoveCity_ContestLobby_Text_ComeThroughHere, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_CONTEST_LOBBY_RECEPTIONIST, SlateportCity_ContestLobby_Movement_ReceptionistEnterContestRoom
	applymovement LOCALID_PLAYER, SlateportCity_ContestLobby_Movement_PlayerEnterContestRoom
	waitmovement 0
	releaseall
	return

SlateportCity_ContestLobby_Movement_PlayerEnterContestRoom:: @ 81554C0
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

SlateportCity_ContestLobby_Movement_PlayerFaceReceptionist:: @ 81554CA
	walk_in_place_fastest_left
	step_end

SlateportCity_ContestLobby_Movement_ReceptionistApproachCounter:: @ 81554CC
	walk_left
	walk_in_place_fastest_down
	step_end

SlateportCity_ContestLobby_Movement_ReceptionistOpenCounter:: @ 81554CF
	walk_down
	walk_down
	walk_in_place_fastest_up
	step_end

SlateportCity_ContestLobby_Movement_ReceptionistEnterContestRoom:: @ 81554D3
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	delay_8
	set_invisible
	step_end

SlateportCity_ContestLobby_Movement_ReceptionistFacePlayer:: @ 81554DC
	walk_in_place_fastest_right
	step_end

SlateportCity_ContestLobby_EventScript_Man:: @ 81554DE
	msgbox SlateportCity_ContestLobby_Text_WantToRaiseUltimateMon, MSGBOX_NPC
	end

SlateportCity_ContestLobby_EventScript_Girl:: @ 81554E7
	msgbox SlateportCity_ContestLobby_Text_DoYourFriendsHaveBerries, MSGBOX_NPC
	end

SlateportCity_ContestLobby_EventScript_Woman:: @ 81554F0
	msgbox SlateportCity_ContestLobby_Text_ContestWinnerPostersOnWall, MSGBOX_NPC
	end

SlateportCity_ContestLobby_EventScript_LittleGirl:: @ 81554F9
	lockall
	goto_if_set FLAG_RECEIVED_POKEBLOCK_CASE, SlateportCity_ContestLobby_EventScript_HavePokeblockCase
	applymovement LOCALID_SLATEPORT_CONTEST_LOBBY_POKEBLOCK_CASE_GIRL, Common_Movement_FacePlayer
	waitmovement 0
	msgbox SlateportCity_ContestLobby_Text_YouCanHavePokeblockCase, MSGBOX_DEFAULT
	giveitem ITEM_POKEBLOCK_CASE
	setflag FLAG_RECEIVED_POKEBLOCK_CASE
	releaseall
	end

SlateportCity_ContestLobby_EventScript_HavePokeblockCase:: @ 8155526
	msgbox SlateportCity_ContestLobby_Text_PutPokeblocksInThere, MSGBOX_NPC
	end

SlateportCity_ContestLobby_EventScript_ShowContestWinner:: @ 815552F
	lockall
	fadescreen FADE_TO_BLACK
	showcontestwinner 3
	releaseall
	end

SlateportCity_ContestLobby_EventScript_Boy:: @ 8155536
	msgbox BerryBlender_Text_LetsGetBlendingAlready, MSGBOX_NPC
	end

@ 815553F
	lockall
	special ShowBerryBlenderRecordWindow
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end
