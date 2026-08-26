ContestHall_MapScripts:: @ 815F580
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, ContestHall_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ContestHall_OnFrame
	map_script MAP_SCRIPT_ON_TRANSITION, ContestHall_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, ContestHall_OnResume
	.byte 0

ContestHall_OnTransition:: @ 815F595
	call ContestHall_EventScript_ReadyContestMusic
	setvar VAR_TEMP_0, 1
	call ContestHall_EventScript_AddRandomAudienceMembers
	end

ContestHall_EventScript_ReadyContestMusic:: @ 815F5A5
	special ScriptGetMultiplayerId
	compare VAR_RESULT, 0
	call_if_eq ContestHall_EventScript_SaveContestMusicPlayer1
	compare VAR_RESULT, 1
	call_if_eq ContestHall_EventScript_SaveContestMusicPlayer2
	compare VAR_RESULT, 2
	call_if_eq ContestHall_EventScript_SaveContestMusicPlayer3
	compare VAR_RESULT, 3
	call_if_eq ContestHall_EventScript_SaveContestMusicPlayer4
	compare VAR_RESULT, 4
	call_if_eq ContestHall_EventScript_SaveContestMusic
	return

ContestHall_EventScript_SaveContestMusicPlayer1:: @ 815F5E0
	savebgm MUS_LINK_CONTEST_P1
	return

ContestHall_EventScript_SaveContestMusicPlayer2:: @ 815F5E4
	savebgm MUS_LINK_CONTEST_P2
	return

ContestHall_EventScript_SaveContestMusicPlayer3:: @ 815F5E8
	savebgm MUS_LINK_CONTEST_P3
	return

ContestHall_EventScript_SaveContestMusicPlayer4:: @ 815F5EC
	savebgm MUS_LINK_CONTEST_P4
	return

ContestHall_EventScript_SaveContestMusic:: @ 815F5F0
	savebgm MUS_CONTEST
	return

ContestHall_OnResume:: @ 815F5F4
	compare VAR_TEMP_9, 1
	call_if_eq ContestHall_EventScript_ReShowAudience
	end

ContestHall_EventScript_ReShowAudience:: @ 815F600
	call ContestHall_EventScript_CreateAudience
	return

ContestHall_OnFrame:: @ 815F606
	map_script_2 VAR_CONTEST_HALL_STATE, 1, ContestHall_EventScript_Contest
	.2byte 0

ContestHall_OnWarp:: @ 815F610
	map_script_2 VAR_CONTEST_HALL_STATE, 1, ContestHall_EventScript_SetContestObjects
	.2byte 0

ContestHall_EventScript_Contest:: @ 815F61A
	call ContestHall_EventScript_DoContest
	call ContestHall_EventScript_SetExitWarp
	end

ContestHall_EventScript_SetContestObjects:: @ 815F625
	hideobjectat LOCALID_PLAYER, MAP_LITTLEROOT_TOWN
	call ContestHall_EventScript_CreateAudience
	end

ContestHall_EventScript_AddRandomAudienceMembers:: @ 815F630
	call ContestHall_EventScript_GetRandomAudienceGfxId
	call ContestHall_EventScript_SetRandomAudienceGfx
	compare VAR_TEMP_0, 8
	goto_if_lt ContestHall_EventScript_AddRandomAudienceMembers
	return

ContestHall_EventScript_SetRandomAudienceGfx:: @ 815F646
	switch VAR_TEMP_0
	case 1, ContestHall_EventScript_SetRandomAudience1
	case 2, ContestHall_EventScript_SetRandomAudience2
	case 3, ContestHall_EventScript_SetRandomAudience3
	case 4, ContestHall_EventScript_SetRandomAudience4
	case 5, ContestHall_EventScript_SetRandomAudience5
	case 6, ContestHall_EventScript_SetRandomAudience6
	case 7, ContestHall_EventScript_SetRandomAudience7
	end

ContestHall_EventScript_SetRandomAudience1:: @ 815F699
	copyvar VAR_OBJ_GFX_ID_4, VAR_TEMP_1
	addvar VAR_TEMP_0, 1
	return

ContestHall_EventScript_SetRandomAudience2:: @ 815F6A4
	copyvar VAR_OBJ_GFX_ID_5, VAR_TEMP_1
	addvar VAR_TEMP_0, 1
	return

ContestHall_EventScript_SetRandomAudience3:: @ 815F6AF
	copyvar VAR_OBJ_GFX_ID_6, VAR_TEMP_1
	addvar VAR_TEMP_0, 1
	return

ContestHall_EventScript_SetRandomAudience4:: @ 815F6BA
	copyvar VAR_OBJ_GFX_ID_7, VAR_TEMP_1
	addvar VAR_TEMP_0, 1
	return

ContestHall_EventScript_SetRandomAudience5:: @ 815F6C5
	copyvar VAR_OBJ_GFX_ID_8, VAR_TEMP_1
	addvar VAR_TEMP_0, 1
	return

ContestHall_EventScript_SetRandomAudience6:: @ 815F6D0
	copyvar VAR_OBJ_GFX_ID_9, VAR_TEMP_1
	addvar VAR_TEMP_0, 1
	return

ContestHall_EventScript_SetRandomAudience7:: @ 815F6DB
	copyvar VAR_OBJ_GFX_ID_A, VAR_TEMP_1
	addvar VAR_TEMP_0, 1
	return

ContestHall_EventScript_GetRandomAudienceGfxId:: @ 815F6E6
	setvar VAR_RESULT, 32
	special ScriptRandom
	addvar VAR_RESULT, 1
	switch VAR_RESULT
	case 1, ContestHall_EventScript_RandomAudienceLittleBoy1
	case 2, ContestHall_EventScript_RandomAudienceLittleGirl1
	case 3, ContestHall_EventScript_RandomAudienceBoy1
	case 4, ContestHall_EventScript_RandomAudienceGirl1
	case 5, ContestHall_EventScript_RandomAudienceGirl2
	case 6, ContestHall_EventScript_RandomAudienceLittleBoy2
	case 7, ContestHall_EventScript_RandomAudienceLittleGirl2
	case 8, ContestHall_EventScript_RandomAudienceBoy3
	case 9, ContestHall_EventScript_RandomAudienceGirl3
	case 10, ContestHall_EventScript_RandomAudienceBoy4
	case 11, ContestHall_EventScript_RandomAudienceFatMan
	case 12, ContestHall_EventScript_RandomAudienceWoman2
	case 13, ContestHall_EventScript_RandomAudienceMan1
	case 14, ContestHall_EventScript_RandomAudienceWoman3
	case 15, ContestHall_EventScript_RandomAudienceOldMan1
	case 16, ContestHall_EventScript_RandomAudienceOldWoman1
	case 17, ContestHall_EventScript_RandomAudienceMan3
	case 18, ContestHall_EventScript_RandomAudienceWoman5
	case 19, ContestHall_EventScript_RandomAudienceCook
	case 20, ContestHall_EventScript_RandomAudienceLass
	case 21, ContestHall_EventScript_RandomAudienceOldWoman2
	case 22, ContestHall_EventScript_RandomAudienceCamper
	case 23, ContestHall_EventScript_RandomAudiencePicnicker
	case 24, ContestHall_EventScript_RandomAudienceMan4
	case 25, ContestHall_EventScript_RandomAudienceWoman7
	case 26, ContestHall_EventScript_RandomAudienceYoungster
	case 27, ContestHall_EventScript_RandomAudienceBugCatcher
	case 28, ContestHall_EventScript_RandomAudiencePsychicM
	case 29, ContestHall_EventScript_RandomAudienceSchoolKidM
	case 30, ContestHall_EventScript_RandomAudienceBlackBelt
	case 31, ContestHall_EventScript_RandomAudienceBeauty
	case 32, ContestHall_EventScript_RandomAudienceScientist1
	end

ContestHall_EventScript_RandomAudienceLittleBoy1:: @ 815F859
	setvar VAR_TEMP_1, 5
	return

ContestHall_EventScript_RandomAudienceLittleGirl1:: @ 815F85F
	setvar VAR_TEMP_1, 6
	return

ContestHall_EventScript_RandomAudienceBoy1:: @ 815F865
	setvar VAR_TEMP_1, 7
	return

ContestHall_EventScript_RandomAudienceGirl1:: @ 815F86B
	setvar VAR_TEMP_1, 8
	return

ContestHall_EventScript_RandomAudienceGirl2:: @ 815F871
	setvar VAR_TEMP_1, 10
	return

ContestHall_EventScript_RandomAudienceLittleBoy2:: @ 815F877
	setvar VAR_TEMP_1, 11
	return

ContestHall_EventScript_RandomAudienceLittleGirl2:: @ 815F87D
	setvar VAR_TEMP_1, 12
	return

ContestHall_EventScript_RandomAudienceBoy3:: @ 815F883
	setvar VAR_TEMP_1, 13
	return

ContestHall_EventScript_RandomAudienceGirl3:: @ 815F889
	setvar VAR_TEMP_1, 14
	return

ContestHall_EventScript_RandomAudienceBoy4:: @ 815F88F
	setvar VAR_TEMP_1, 15
	return

ContestHall_EventScript_RandomAudienceFatMan:: @ 815F895
	setvar VAR_TEMP_1, 17
	return

ContestHall_EventScript_RandomAudienceWoman2:: @ 815F89B
	setvar VAR_TEMP_1, 18
	return

ContestHall_EventScript_RandomAudienceMan1:: @ 815F8A1
	setvar VAR_TEMP_1, 19
	return

ContestHall_EventScript_RandomAudienceWoman3:: @ 815F8A7
	setvar VAR_TEMP_1, 20
	return

ContestHall_EventScript_RandomAudienceOldMan1:: @ 815F8AD
	setvar VAR_TEMP_1, 21
	return

ContestHall_EventScript_RandomAudienceOldWoman1:: @ 815F8B3
	setvar VAR_TEMP_1, 22
	return

ContestHall_EventScript_RandomAudienceMan3:: @ 815F8B9
	setvar VAR_TEMP_1, 25
	return

ContestHall_EventScript_RandomAudienceWoman5:: @ 815F8BF
	setvar VAR_TEMP_1, 26
	return

ContestHall_EventScript_RandomAudienceCook:: @ 815F8C5
	setvar VAR_TEMP_1, 27
	return

ContestHall_EventScript_RandomAudienceLass:: @ 815F8CB
	setvar VAR_TEMP_1, 47
	return

ContestHall_EventScript_RandomAudienceOldWoman2:: @ 815F8D1
	setvar VAR_TEMP_1, 30
	return

ContestHall_EventScript_RandomAudienceCamper:: @ 815F8D7
	setvar VAR_TEMP_1, 31
	return

ContestHall_EventScript_RandomAudiencePicnicker:: @ 815F8DD
	setvar VAR_TEMP_1, 32
	return

ContestHall_EventScript_RandomAudienceMan4:: @ 815F8E3
	setvar VAR_TEMP_1, 33
	return

ContestHall_EventScript_RandomAudienceWoman7:: @ 815F8E9
	setvar VAR_TEMP_1, 34
	return

ContestHall_EventScript_RandomAudienceYoungster:: @ 815F8EF
	setvar VAR_TEMP_1, 35
	return

ContestHall_EventScript_RandomAudienceBugCatcher:: @ 815F8F5
	setvar VAR_TEMP_1, 36
	return

ContestHall_EventScript_RandomAudiencePsychicM:: @ 815F8FB
	setvar VAR_TEMP_1, 37
	return

ContestHall_EventScript_RandomAudienceSchoolKidM:: @ 815F901
	setvar VAR_TEMP_1, 38
	return

ContestHall_EventScript_RandomAudienceBlackBelt:: @ 815F907
	setvar VAR_TEMP_1, 44
	return

ContestHall_EventScript_RandomAudienceBeauty:: @ 815F90D
	setvar VAR_TEMP_1, 45
	return

ContestHall_EventScript_RandomAudienceScientist1:: @ 815F913
	setvar VAR_TEMP_1, 46
	return

ContestHall_EventScript_CreateAudience:: @ 815F919
	switch VAR_CONTEST_LOCATION
	case 1, ContestHall_EventScript_CreateNormalContestAudience
	case 2, ContestHall_EventScript_CreateSuperContestAudience
	case 3, ContestHall_EventScript_CreateHyperContestAudience
	case 4, ContestHall_EventScript_CreateMasterContestAudience
	case 5, ContestHall_EventScript_CreateMasterContestAudience
	return

ContestHall_EventScript_CreateNormalContestAudience:: @ 815F956
	createvobject 5, 20, 3, 2, 3, 1
	createvobject 46, 24, 11, 2, 3, 1
	return

ContestHall_EventScript_CreateSuperContestAudience:: @ 815F969
	createvobject 45, 0, 2, 3, 3, 4
	createvobject 66, 1, 2, 4, 3, 4
	createvobject 55, 2, 2, 7, 3, 4
	createvobject 46, 3, 2, 8, 3, 4
	createvobject 5, 10, 12, 3, 3, 3
	createvobject 20, 11, 12, 4, 3, 3
	createvobject 24, 12, 12, 7, 3, 3
	createvobject 12, 13, 12, 8, 3, 3
	createvobject 47, 20, 3, 2, 3, 1
	createvobject 46, 24, 11, 2, 3, 1
	return

ContestHall_EventScript_CreateHyperContestAudience:: @ 815F9C4
	createvobject 45, 0, 2, 3, 3, 4
	createvobject 66, 1, 2, 4, 3, 4
	createvobject 55, 2, 2, 7, 3, 4
	createvobject 48, 3, 2, 8, 3, 4
	createvobject 5, 10, 12, 3, 3, 3
	createvobject 20, 11, 12, 4, 3, 3
	createvobject 24, 12, 12, 7, 3, 3
	createvobject 45, 13, 12, 8, 3, 3
	createvobject 22, 20, 3, 2, 3, 1
	createvobject 23, 20, 6, 2, 3, 1
	createvobject 34, 20, 7, 2, 3, 1
	createvobject 46, 24, 8, 2, 3, 1
	createvobject 48, 24, 11, 2, 3, 1
	createvobject 11, 25, 3, 9, 3, 2
	createvobject 35, 26, 4, 9, 3, 2
	createvobject 17, 27, 5, 9, 3, 2
	createvobject 41, 28, 9, 9, 3, 2
	createvobject 38, 29, 10, 9, 3, 2
	createvobject 83, 30, 11, 9, 3, 2
	return

ContestHall_EventScript_CreateMasterContestAudience:: @ 815FA70
	createvobject 45, 0, 2, 3, 3, 4
	createvobject 66, 1, 2, 4, 3, 4
	createvobject 55, 2, 2, 7, 3, 4
	createvobject 12, 3, 2, 8, 3, 4
	createvobject 39, 4, 1, 3, 3, 4
	createvobject 34, 6, 1, 5, 3, 4
	createvobject 26, 7, 1, 6, 3, 4
	createvobject 48, 9, 1, 8, 3, 4
	createvobject 5, 10, 12, 3, 3, 3
	createvobject 20, 11, 12, 4, 3, 3
	createvobject 24, 12, 12, 7, 3, 3
	createvobject 45, 13, 12, 8, 3, 3
	createvobject 50, 14, 13, 3, 3, 3
	createvobject 52, 15, 13, 4, 3, 3
	createvobject 65, 17, 13, 6, 3, 3
	createvobject 83, 18, 13, 7, 3, 3
	createvobject 116, 19, 13, 8, 3, 3
	createvobject 25, 20, 3, 2, 3, 1
	createvobject 31, 21, 6, 2, 3, 1
	createvobject 33, 22, 7, 2, 3, 1
	createvobject 46, 24, 11, 2, 3, 1
	createvobject 49, 25, 3, 9, 3, 2
	createvobject 35, 26, 4, 9, 3, 2
	createvobject 48, 27, 5, 9, 3, 2
	createvobject 41, 28, 9, 9, 3, 2
	createvobject 38, 29, 10, 9, 3, 2
	createvobject 83, 30, 11, 9, 3, 2
	return

ContestHall_EventScript_SetExitWarp:: @ 815FB64
	switch VAR_CONTEST_LOCATION
	case 1, ContestHall_EventScript_SetExitWarpNormalContest
	case 2, ContestHall_EventScript_SetExitWarpSuperContest
	case 3, ContestHall_EventScript_SetExitWarpHyperContest
	case 4, ContestHall_EventScript_SetExitWarpMasterContest
	case 5, ContestHall_EventScript_SetExitWarpLinkContest
	return

ContestHall_EventScript_SetExitWarpNormalContest:: @ 815FBA1
	incrementgamestat GAME_STAT_ENTERED_CONTEST
	warp MAP_VERDANTURF_TOWN_CONTEST_LOBBY, 255, 5, 4
	waitstate
	end

ContestHall_EventScript_SetExitWarpSuperContest:: @ 815FBAD
	incrementgamestat GAME_STAT_ENTERED_CONTEST
	warp MAP_FALLARBOR_TOWN_CONTEST_LOBBY, 255, 5, 4
	waitstate
	end

ContestHall_EventScript_SetExitWarpHyperContest:: @ 815FBB9
	incrementgamestat GAME_STAT_ENTERED_CONTEST
	warp MAP_SLATEPORT_CITY_CONTEST_LOBBY, 255, 5, 4
	waitstate
	end

ContestHall_EventScript_SetExitWarpMasterContest:: @ 815FBC5
	incrementgamestat GAME_STAT_ENTERED_CONTEST
	warp MAP_LILYCOVE_CITY_CONTEST_LOBBY, 255, 6, 4
	waitstate
	end

ContestHall_EventScript_SetExitWarpLinkContest:: @ 815FBD1
	warp MAP_LILYCOVE_CITY_CONTEST_LOBBY, 255, 7, 4
	waitstate
	end

LilycoveCity_ContestLobby_EventScript_SetupRivalGfxId:: @ 815FBDB
	checkplayergender
	compare VAR_RESULT, 0
	goto_if_eq FallarborTown_ContestLobby_EventScript_SetupRivalGfxIdMale
	compare VAR_RESULT, 1
	goto_if_eq FallarborTown_ContestLobby_EventScript_SetupRivalGfxIdFemale
	return

FallarborTown_ContestLobby_EventScript_SetupRivalGfxIdMale:: @ 815FBF3
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

FallarborTown_ContestLobby_EventScript_SetupRivalGfxIdFemale:: @ 815FBF9
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return
