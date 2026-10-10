Interview_EventScript_EndInterview:: @ 81ADE46
	special InterviewAfter
	incrementgamestat GAME_STAT_GOT_INTERVIEWED
	release
	end

SlateportCity_PokemonFanClub_EventScript_ReporterNoNickname:: @ 81ADE4D
	setvar VAR_0x8005, 1
	special InterviewBefore
	compare VAR_RESULT, 1
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed2
	copyvar VAR_0x8009, VAR_0x8006
	msgbox SlateportCity_PokemonFanClub_Text_InterviewRequest, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_AcceptInterview2
	compare VAR_RESULT, NO
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_DeclineInterview2
	end

SlateportCity_PokemonFanClub_EventScript_AcceptInterview2:: @ 81ADE84
	msgbox SlateportCity_PokemonFanClub_Text_TellMeAnythingAboutYourMon, MSGBOX_DEFAULT
	setvar VAR_0x8004, 5
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, 1
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 1
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_SubmitResponse2
	compare VAR_RESULT, 0
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_DeclineInterview2
	end

SlateportCity_PokemonFanClub_EventScript_DeclineInterview2:: @ 81ADEB9
	msgbox SlateportCity_PokemonFanClub_Text_HereIfYouGetUrgeToTellMe, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_SubmitResponse2:: @ 81ADEC3
	msgbox SlateportCity_PokemonFanClub_Text_ThatsAllForInterview2, MSGBOX_DEFAULT
	setvar VAR_0x8005, 1
	goto Interview_EventScript_EndInterview
	end

SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed2:: @ 81ADED6
	msgbox SlateportCity_PokemonFanClub_Text_EnjoyDoingInterviews, MSGBOX_DEFAULT
	release
	end

SlateportCity_OceanicMuseum_1F_EventScript_Reporter:: @ 81ADEE0
	lock
	faceplayer
	setvar VAR_0x8005, 2
	special InterviewBefore
	compare VAR_RESULT, 1
	goto_if_eq SlateportCity_OceanicMuseum_1F_EventScript_AlreadyInterviewed
	copyvar VAR_0x8009, VAR_0x8006
	goto_if_set FLAG_OCEANIC_MUSEUM_MET_REPORTER, SlateportCity_OceanicMuseum_1F_EventScript_RequestInterviewShort
	setflag FLAG_OCEANIC_MUSEUM_MET_REPORTER
	msgbox SlateportCity_OceanicMuseum_1F_Text_InterviewRequest, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SlateportCity_OceanicMuseum_1F_EventScript_AcceptInterview
	compare VAR_RESULT, NO
	goto_if_eq SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_RequestInterviewShort:: @ 81ADF25
	msgbox SlateportCity_OceanicMuseum_1F_Text_InterviewRequestShort, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SlateportCity_OceanicMuseum_1F_EventScript_AcceptInterview
	compare VAR_RESULT, NO
	goto_if_eq SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_AcceptInterview:: @ 81ADF44
	msgbox SlateportCity_OceanicMuseum_1F_Text_TellMeExperienceInvolvingPokemon, MSGBOX_DEFAULT
	setvar VAR_0x8004, 5
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, 0
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 1
	goto_if_eq SlateportCity_OceanicMuseum_1F_EventScript_SubmitResponse
	compare VAR_RESULT, 0
	goto_if_eq SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview:: @ 81ADF79
	msgbox SlateportCity_OceanicMuseum_1F_Text_LetMeKnowIfYouHaveStory, MSGBOX_DEFAULT
	release
	end

SlateportCity_OceanicMuseum_1F_EventScript_SubmitResponse:: @ 81ADF83
	msgbox SlateportCity_OceanicMuseum_1F_Text_ThatsAllForInterview, MSGBOX_DEFAULT
	setvar VAR_0x8005, 2
	goto Interview_EventScript_EndInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_AlreadyInterviewed:: @ 81ADF96
	msgbox SlateportCity_OceanicMuseum_1F_Text_BetterWriteUpStory, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_Reporter:: @ 81ADFA0
	lock
	faceplayer
	specialvar VAR_RESULT, LeadMonNicknamed
	compare VAR_RESULT, 0
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_ReporterNoNickname
	setvar VAR_0x8005, 3
	special InterviewBefore
	compare VAR_RESULT, 1
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed
	copyvar VAR_0x8009, VAR_0x8006
	msgbox SlateportCity_PokemonFanClub_Text_InterviewRequestHasName, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_AcceptInterview
	compare VAR_RESULT, NO
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_DeclineInterview
	end

SlateportCity_PokemonFanClub_EventScript_AcceptInterview:: @ 81ADFE9
	msgbox SlateportCity_PokemonFanClub_Text_HereGoesQuickAnswers, MSGBOX_DEFAULT
	random 3
	copyvar VAR_0x800A, VAR_RESULT
	switch VAR_RESULT
	case 0, SlateportCity_PokemonFanClub_EventScript_RandomQuestion1
	case 1, SlateportCity_PokemonFanClub_EventScript_RandomQuestion2
	case 2, SlateportCity_PokemonFanClub_EventScript_RandomQuestion3
	end

SlateportCity_PokemonFanClub_EventScript_RandomQuestion1:: @ 81AE020
	msgbox SlateportCity_PokemonFanClub_Text_DescribeFeelingsFirstMetMon, MSGBOX_DEFAULT
	goto SlateportCity_PokemonFanClub_EventScript_ContinueInterview
	end

SlateportCity_PokemonFanClub_EventScript_RandomQuestion2:: @ 81AE02E
	msgbox SlateportCity_PokemonFanClub_Text_LikenMonToSomethingYouLike, MSGBOX_DEFAULT
	goto SlateportCity_PokemonFanClub_EventScript_ContinueInterview
	end

SlateportCity_PokemonFanClub_EventScript_RandomQuestion3:: @ 81AE03C
	msgbox SlateportCity_PokemonFanClub_Text_WhatAttractedYouAboutMon, MSGBOX_DEFAULT
	goto SlateportCity_PokemonFanClub_EventScript_ContinueInterview
	end

SlateportCity_PokemonFanClub_EventScript_ContinueInterview:: @ 81AE04A
	setvar VAR_0x8004, 7
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, 0
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 0
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_DeclineInterview
	msgbox SlateportCity_PokemonFanClub_Text_WhatDoPokemonMeanToYou, MSGBOX_DEFAULT
	setvar VAR_0x8006, 1
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 0
	goto_if_eq SlateportCity_PokemonFanClub_EventScript_DeclineInterview
	msgbox SlateportCity_PokemonFanClub_Text_ThatsAllForInterview, MSGBOX_DEFAULT
	copyvar VAR_0x8007, VAR_0x800A
	setvar VAR_0x8005, 3
	goto Interview_EventScript_EndInterview
	end

SlateportCity_PokemonFanClub_EventScript_DeclineInterview:: @ 81AE0A2
	msgbox SlateportCity_PokemonFanClub_Text_HereIfYouGetUrgeToTellMe, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed:: @ 81AE0AC
	msgbox SlateportCity_PokemonFanClub_Text_EnjoyDoingInterviews, MSGBOX_DEFAULT
	release
	end

FallarborTown_ContestLobby_EventScript_Reporter:: @ 81AE0B6
LilycoveCity_ContestLobby_EventScript_Reporter:: @ 81AE0B6
SlateportCity_ContestLobby_EventScript_Reporter:: @ 81AE0B6
VerdanturfTown_ContestLobby_EventScript_1AE0B6:: @ 81AE0B6
	lock
	faceplayer
	goto_if_set FLAG_TEMP_2, FallarborTown_ContestLobby_EventScript_1AE17E
	setvar VAR_0x8005, 6
	special InterviewBefore
	compare VAR_RESULT, 1
	goto_if_eq FallarborTown_ContestLobby_EventScript_1AE17E
	copyvar VAR_0x8009, VAR_0x8006
	msgbox FallarborTown_ContestLobby_Text_1A6F7C, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq FallarborTown_ContestLobby_EventScript_1AE0F8
	compare VAR_RESULT, NO
	goto_if_eq FallarborTown_ContestLobby_EventScript_1AE12D
	end

FallarborTown_ContestLobby_EventScript_1AE0F8:: @ 81AE0F8
	msgbox FallarborTown_ContestLobby_Text_1A704E, MSGBOX_DEFAULT
	setvar VAR_0x8004, 11
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, 0
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 1
	goto_if_eq FallarborTown_ContestLobby_EventScript_1AE137
	compare VAR_RESULT, 0
	goto_if_eq FallarborTown_ContestLobby_EventScript_1AE12D
	end

FallarborTown_ContestLobby_EventScript_1AE12D:: @ 81AE12D
	msgbox FallarborTown_ContestLobby_Text_1A7256, MSGBOX_DEFAULT
	release
	end

FallarborTown_ContestLobby_EventScript_1AE137:: @ 81AE137
	setvar VAR_0x8004, 24
	special SetContestCategoryStringVarForInterview
	msgbox FallarborTown_ContestLobby_Text_1A70A5, MSGBOX_DEFAULT
	setvar VAR_0x8004, 11
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, 1
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 0
	goto_if_eq FallarborTown_ContestLobby_EventScript_1AE12D
	msgbox FallarborTown_ContestLobby_Text_1A7153, MSGBOX_DEFAULT
	setflag FLAG_TEMP_2
	setvar VAR_0x8005, 6
	goto Interview_EventScript_EndInterview
	end

FallarborTown_ContestLobby_EventScript_1AE17E:: @ 81AE17E
	msgbox FallarborTown_ContestLobby_Text_1A72A8, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_TryShowContestReporter:: @ 81AE188
	compare VAR_CONTEST_HALL_STATE, 2
	goto_if_ne LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	setvar VAR_0x8005, 6
	special InterviewBefore
	compare VAR_RESULT, 1
	goto_if_eq LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	switch VAR_CONTEST_TYPE
	case 0, LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	case 2, FallarborTown_ContestLobby_EventScript_ShowFallarborContestReporter
	case 1, FallarborTown_ContestLobby_EventScript_ShowVerdanturfContestReporter
	case 3, FallarborTown_ContestLobby_EventScript_ShowSlateportContestReporter
	case 4, FallarborTown_ContestLobby_EventScript_ShowLilycoveContestReporter
	case 5, LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	end

FallarborTown_ContestLobby_EventScript_ShowFallarborContestReporter:: @ 81AE1EE
	clearflag FLAG_HIDE_CONTEST_REPORTER_FALLARBOR
	return

FallarborTown_ContestLobby_EventScript_ShowVerdanturfContestReporter:: @ 81AE1F2
	clearflag FLAG_HIDE_CONTEST_REPORTER_VERDANTURF
	return

FallarborTown_ContestLobby_EventScript_ShowSlateportContestReporter:: @ 81AE1F6
	clearflag FLAG_HIDE_SLATEPORT_CITY_CONTEST_REPORTER
	return

FallarborTown_ContestLobby_EventScript_ShowLilycoveContestReporter:: @ 81AE1FA
	clearflag FLAG_HIDE_LILYCOVE_CONTEST_HALL_REPORTER
	return

LilycoveCity_ContestLobby_EventScript_DontShowContestReporter:: @ 81AE1FE
	return

BattleTower_Lobby_EventScript_1AE1FF:: @ 81AE1FF
	lock
	faceplayer
	goto_if_set FLAG_TEMP_2, BattleTower_Lobby_EventScript_1AE2E3
	setvar VAR_0x8005, 7
	special InterviewBefore
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Lobby_EventScript_1AE2E3
	copyvar VAR_0x8009, VAR_0x8006
	msgbox BattleTower_Lobby_Text_1A776D, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq BattleTower_Lobby_EventScript_1AE241
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Lobby_EventScript_1AE297
	end

BattleTower_Lobby_EventScript_1AE241:: @ 81AE241
	message BattleTower_Lobby_Text_1A7823
	waitmessage
	multichoice 19, 8, 45, 1
	copyvar VAR_0x8008, VAR_RESULT
	compare VAR_RESULT, 0
	call_if_eq BattleTower_Lobby_EventScript_1AE2A1
	compare VAR_RESULT, 1
	call_if_eq BattleTower_Lobby_EventScript_1AE2AA
	msgbox BattleTower_Lobby_Text_1A79EB, MSGBOX_DEFAULT
	setvar VAR_0x8004, 12
	copyvar VAR_0x8005, VAR_0x8009
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Lobby_EventScript_1AE2B3
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_1AE2D9
	end

BattleTower_Lobby_EventScript_1AE297:: @ 81AE297
	msgbox BattleTower_Lobby_Text_1A78B7, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_1AE2A1:: @ 81AE2A1
	msgbox BattleTower_Lobby_Text_1A791B, MSGBOX_DEFAULT
	return

BattleTower_Lobby_EventScript_1AE2AA:: @ 81AE2AA
	msgbox BattleTower_Lobby_Text_1A7990, MSGBOX_DEFAULT
	return

BattleTower_Lobby_EventScript_1AE2B3:: @ 81AE2B3
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_1AE2D9
	msgbox BattleTower_Lobby_Text_1A7A6E, MSGBOX_DEFAULT
	setflag FLAG_TEMP_2
	copyvar VAR_0x8004, VAR_0x8008
	setvar VAR_0x8005, 7
	goto Interview_EventScript_EndInterview
	end

BattleTower_Lobby_EventScript_1AE2D9:: @ 81AE2D9
	msgbox BattleTower_Lobby_Text_1A7AE0, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_1AE2E3:: @ 81AE2E3
	msgbox BattleTower_Lobby_Text_1A7B66, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_1AE2ED:: @ 81AE2ED
	compare VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 0
	goto_if_eq BattleTower_Lobby_EventScript_1AE30F
	setvar VAR_0x8005, 7
	special InterviewBefore
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Lobby_EventScript_1AE30F
	clearflag FLAG_HIDE_BATTLE_TOWER_REPORTER
	return

BattleTower_Lobby_EventScript_1AE30F:: @ 81AE30F
	setflag FLAG_HIDE_BATTLE_TOWER_REPORTER
	return
