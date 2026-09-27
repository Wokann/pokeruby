ContestLobby_EventScript_SpeakToContestReceptionist:: @ 81A4E92
	lock
	faceplayer
	checkitem ITEM_CONTEST_PASS
	call_if_eq VAR_RESULT, FALSE, ContestLobby_EventScript_ResetContestPassPrompt
	goto_if_ne VAR_CONTEST_PRIZE_PICKUP, FALSE, ContestLobby_EventScript_PickUpPrize
	goto_if_set FLAG_TEMP_1, ContestLobby_EventScript_AskEnterContest
	bufferstdstring 0, VAR_0x800B
	msgbox ContestLobby_Text_RegistrationForRankContests, MSGBOX_DEFAULT
	checkitem ITEM_CONTEST_PASS
	goto_if_eq VAR_RESULT, FALSE, ContestLobby_EventScript_NoContestPass
	setflag FLAG_TEMP_1
	goto ContestLobby_EventScript_AskEnterContest
	end

ContestLobby_EventScript_ResetContestPassPrompt:: @ 81A4EDD
	clearflag FLAG_TEMP_1
	return

ContestLobby_EventScript_NoContestPass:: @ 81A4EE1
	compare VAR_CONTEST_RANK, CONTEST_RANK_NORMAL
	goto_if_eq ContestLobby_EventScript_AskToIssueContestPass
	msgbox ContestLobby_Text_GetContestPassAtVerdanturf, MSGBOX_DEFAULT
	releaseall
	end

ContestLobby_EventScript_AskToIssueContestPass:: @ 81A4EF6
	msgbox ContestLobby_Text_AskToIssueContestPass, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq ContestLobby_EventScript_GiveContestPass
	msgbox ContestLobby_Text_ComeBackToEnterContest, MSGBOX_DEFAULT
	releaseall
	end

ContestLobby_EventScript_GiveContestPass:: @ 81A4F13
	goto_if_set FLAG_RECEIVED_CONTEST_PASS, ContestLobby_EventScript_AlreadyIssuedContestPass
	msgbox ContestLobby_Text_GiveContestPass, MSGBOX_DEFAULT
	giveitem ITEM_CONTEST_PASS
	setflag FLAG_RECEIVED_CONTEST_PASS
	setflag FLAG_TEMP_1
	msgbox ContestLobby_Text_ExplainContests, MSGBOX_DEFAULT
	goto ContestLobby_EventScript_AskEnterContest
	end

ContestLobby_EventScript_AlreadyIssuedContestPass:: @ 81A4F44
	msgbox ContestLobby_Text_AlreadyIssuedContestPass, MSGBOX_DEFAULT
	releaseall
	end

ContestLobby_EventScript_PickUpPrize:: @ 81A4F4E
	msgbox ContestLobby_Text_PokemonWonWeHavePrize, MSGBOX_DEFAULT
	switch VAR_CONTEST_PRIZE_PICKUP
	case 4, ContestLobby_EventScript_GiveLuxuryBallAtCounter
	end

ContestLobby_EventScript_GiveLuxuryBallAtCounter:: @ 81A4F67
	giveitem ITEM_LUXURY_BALL
	compare VAR_RESULT, FALSE
	goto_if_eq ContestLobby_EventScript_NoRoomForLuxuryBallAtCounter
	setvar VAR_CONTEST_PRIZE_PICKUP, FALSE
	closemessage
	release
	end

ContestLobby_EventScript_NoRoomForLuxuryBallAtCounter:: @ 81A4F86
	msgbox ContestLobby_Text_ComeBackForPrizeLater, MSGBOX_NPC
	end

ContestLobby_EventScript_AskEnterContest:: @ 81A4F8F
	message ContestLobby_Text_EnterContest1
	waitmessage
	multichoice 0, 0, MULTI_ENTERINFO, FALSE
	switch VAR_RESULT
	case 0, ContestLobby_EventScript_ChooseContestType
	case 1, ContestLobby_EventScript_ContestInfo
	case 2, ContestLobby_EventScript_CancelEnterContest
	case MULTI_B_PRESSED, ContestLobby_EventScript_CancelEnterContest
	end

ContestLobby_EventScript_ContestInfo:: @ 81A4FCC
	message ContestLobby_Text_WhichTopic1
	waitmessage
	multichoice 0, 0, MULTI_CONTEST_INFO, FALSE
	switch VAR_RESULT
	case 0, ContestLobby_EventScript_ExplainContests
	case 1, ContestLobby_EventScript_ExplainContestTypes
	case 2, ContestLobby_EventScript_ExplainContestRanks
	case 3, ContestLobby_EventScript_AskEnterContest
	case MULTI_B_PRESSED, ContestLobby_EventScript_AskEnterContest
	end

ContestLobby_EventScript_ExplainContests:: @ 81A5014
	msgbox ContestLobby_Text_ExplainContests, MSGBOX_DEFAULT
	goto ContestLobby_EventScript_ContestInfo
	end

ContestLobby_EventScript_ExplainContestTypes:: @ 81A5022
	msgbox ContestLobby_Text_ExplainContestTypes, MSGBOX_DEFAULT
	goto ContestLobby_EventScript_ContestInfo
	end

ContestLobby_EventScript_ExplainContestRanks:: @ 81A5030
	msgbox ContestLobby_Text_ExplainContestRanks, MSGBOX_DEFAULT
	goto ContestLobby_EventScript_ContestInfo
	end

ContestLobby_EventScript_CancelEnterContest:: @ 81A503E
	msgbox ContestLobby_Text_ParticipateAnotherTime, MSGBOX_DEFAULT
	release
	end

ContestLobby_EventScript_ChooseContestMon:: @ 81A5048
	msgbox ContestLobby_Text_EnterWhichPokemon1, MSGBOX_DEFAULT
	choosecontestmon
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq ContestLobby_EventScript_ChooseContestType
	special TryEnterContestMon
	compare VAR_RESULT, CANT_ENTER_CONTEST
	goto_if_eq ContestLobby_EventScript_CantEnterLowRank
	compare VAR_RESULT, CAN_ENTER_CONTEST_EQUAL_RANK
	goto_if_eq ContestLobby_EventScript_ConfirmEntry
	compare VAR_RESULT, CAN_ENTER_CONTEST_HIGH_RANK
	goto_if_eq ContestLobby_EventScript_ConfirmEntryAlreadyWon
	compare VAR_RESULT, CANT_ENTER_CONTEST_EGG
	goto_if_eq ContestLobby_EventScript_CantEnterEgg
	compare VAR_RESULT, CANT_ENTER_CONTEST_FAINTED
	goto_if_eq ContestLobby_EventScript_CantEnterFainted
	end

ContestLobby_EventScript_ChooseContestType:: @ 81A5097
	message ContestLobby_Text_EnterWhichContest1
	waitmessage
	multichoice 0, 0, MULTI_CONTEST_TYPE, FALSE
	switch VAR_RESULT
	case CONTEST_CATEGORIES_COUNT, ContestLobby_EventScript_CancelEnterContest
	case MULTI_B_PRESSED, ContestLobby_EventScript_CancelEnterContest
	copyvar VAR_CONTEST_CATEGORY, VAR_RESULT
	goto ContestLobby_EventScript_ChooseContestMon
	end

ContestLobby_EventScript_CantEnterLowRank:: @ 81A50C8
	msgbox ContestLobby_Text_MonNotQualifiedForRank, MSGBOX_DEFAULT
	goto ContestLobby_EventScript_ChooseContestMon
	release
	end

ContestLobby_EventScript_ConfirmEntry:: @ 81A50D7
	msgbox ContestLobby_Text_ConfirmContestMon, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, ContestLobby_EventScript_ChooseContestMon
	case YES, ContestLobby_EventScript_EnterMon
	end

ContestLobby_EventScript_ConfirmEntryAlreadyWon:: @ 81A50FB
	msgbox ContestLobby_Text_AlreadyWonEnterAnyway, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, ContestLobby_EventScript_ChooseContestMon
	case YES, ContestLobby_EventScript_EnterMon
	end

ContestLobby_EventScript_CantEnterEgg:: @ 81A511F
	msgbox ContestLobby_Text_EggCannotTakePart, MSGBOX_DEFAULT
	goto ContestLobby_EventScript_ChooseContestMon
	release
	end

ContestLobby_EventScript_CantEnterFainted:: @ 81A512E
	msgbox ContestLobby_Text_MonInNoConditionForContest, MSGBOX_DEFAULT
	goto ContestLobby_EventScript_ChooseContestMon
	release
	end

ContestLobby_EventScript_EnterMon:: @ 81A513D
	msgbox ContestLobby_Text_YourMonIsEntryNum4, MSGBOX_DEFAULT
	closemessage
	releaseall
	setvar VAR_LINK_CONTEST_ROOM_STATE, 1
	return

LinkContestRoom1_EventScript_RunContest:: @ 81A514D
	setvar VAR_0x8006, 0
	lockall
	applymovement 14, LinkContestRoom1_Movement_HostFaceUp
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_FaceDown
	waitmovement 0
	releaseall
	call LinkContestRoom1_EventScript_SetRankBuffer
	call LinkContestRoom1_EventScript_SetCategoryBuffer
	call LinkContestRoom1_EventScript_AnnounceContestOpening
	call LinkContestRoom1_EventScript_IntroduceContestants
	call LinkContestRoom1_EventScript_StartSecondaryJudging
	call LinkContestRoom1_EventScript_AnnounceJudgingResults
	call LinkContestRoom1_EventScript_SetWinnerObject
	call LinkContestRoom1_EventScript_AnnounceWinner
	call LinkContestRoom1_EventScript_CheerForWinner
	call LinkContestRoom1_EventScript_AwardContestWinner
	setvar VAR_LINK_CONTEST_ROOM_STATE, 2
	return

LinkContestRoom1_EventScript_SetRankBuffer:: @ 81A51A0
	switch VAR_CONTEST_RANK
	case 0, LinkContestRoom1_EventScript_SetNormalRankBuffer
	case 1, LinkContestRoom1_EventScript_SetSuperRankBuffer
	case 2, LinkContestRoom1_EventScript_SetHyperRankBuffer
	case 3, LinkContestRoom1_EventScript_SetMasterRankBuffer
	return

LinkContestRoom1_EventScript_SetNormalRankBuffer:: @ 81A51D2
	setvar VAR_0x8009, 5
	return

LinkContestRoom1_EventScript_SetSuperRankBuffer:: @ 81A51D8
	setvar VAR_0x8009, 6
	return

LinkContestRoom1_EventScript_SetHyperRankBuffer:: @ 81A51DE
	setvar VAR_0x8009, 7
	return

LinkContestRoom1_EventScript_SetMasterRankBuffer:: @ 81A51E4
	setvar VAR_0x8009, 8
	return

LinkContestRoom1_EventScript_SetCategoryBuffer:: @ 81A51EA
	switch VAR_CONTEST_CATEGORY
	case 0, LinkContestRoom1_EventScript_SetCoolCategoryBuffer
	case 1, LinkContestRoom1_EventScript_SetBeautyCategoryBuffer
	case 2, LinkContestRoom1_EventScript_SetCuteCategoryBuffer
	case 3, LinkContestRoom1_EventScript_SetSmartCategoryBuffer
	case 4, LinkContestRoom1_EventScript_SetToughCategoryBuffer
	return

LinkContestRoom1_EventScript_SetCoolCategoryBuffer:: @ 81A5227
	setvar VAR_0x8008, 0
	return

LinkContestRoom1_EventScript_SetBeautyCategoryBuffer:: @ 81A522D
	setvar VAR_0x8008, 1
	return

LinkContestRoom1_EventScript_SetCuteCategoryBuffer:: @ 81A5233
	setvar VAR_0x8008, 2
	return

LinkContestRoom1_EventScript_SetSmartCategoryBuffer:: @ 81A5239
	setvar VAR_0x8008, 3
	return

LinkContestRoom1_EventScript_SetToughCategoryBuffer:: @ 81A523F
	setvar VAR_0x8008, 4
	return

LinkContestRoom1_EventScript_AnnounceContestOpening:: @ 81A5245
	bufferstdstring 1, VAR_0x8008
	bufferstdstring 2, VAR_0x8009
	call LinkContestRoom1_EventScript_AnnounceContestOpeningAtHall
	lockall
	applymovement 1, LinkContestRoom1_Movement_WalkUpFacingLocked
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_AnnounceContestOpeningAtHall:: @ 81A525F
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnounceContestOpeningOverLink
	msgbox LinkContestRoom1_Text_OpeningAnnouncement, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_AnnounceContestOpeningOverLink:: @ 81A5273
	messageautoscroll LinkContestRoom1_Text_LinkOpeningAnnouncement
	waitmessage
	return

LinkContestRoom1_EventScript_IntroduceContestants:: @ 81A527A
	call LinkContestRoom1_EventScript_PositionNextContestant
	call LinkContestRoom1_EventScript_IntroduceContestant
	call LinkContestRoom1_EventScript_SetAudienceReaction
	call LinkContestRoom1_EventScript_ShowContestantToAudience
	call LinkContestRoom1_EventScript_ReturnContestantToPlace
	addvar VAR_0x8006, 1
	compare VAR_0x8006, 4
	goto_if_ne LinkContestRoom1_EventScript_IntroduceContestants
	call LinkContestRoom1_EventScript_BeginPrimaryJudging
	setvar VAR_TEMP_1, 6
	return

LinkContestRoom1_EventScript_PositionNextContestant:: @ 81A52AE
	compare VAR_0x8006, 0
	goto_if_eq LinkContestRoom1_EventScript_PositionContestantOne
	compare VAR_0x8006, 1
	goto_if_eq LinkContestRoom1_EventScript_PositionContestantTwo
	compare VAR_0x8006, 2
	goto_if_eq LinkContestRoom1_EventScript_PositionContestantThree
	compare VAR_0x8006, 3
	goto_if_eq LinkContestRoom1_EventScript_PositionContestantFour
	return

LinkContestRoom1_EventScript_PositionContestantOne:: @ 81A52DB
	lockall
	applymovement 3, LinkContestRoom1_Movement_ContestantOneEnter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 3
	return

LinkContestRoom1_EventScript_PositionContestantTwo:: @ 81A52ED
	lockall
	applymovement 4, LinkContestRoom1_Movement_ContestantTwoEnter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 4
	return

LinkContestRoom1_EventScript_PositionContestantThree:: @ 81A52FF
	lockall
	applymovement 5, LinkContestRoom1_Movement_ContestantThreeEnter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 5
	return

LinkContestRoom1_EventScript_PositionContestantFour:: @ 81A5311
	lockall
	applymovement 14, LinkContestRoom1_Movement_ContestantFourEnter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 14
	return

LinkContestRoom1_EventScript_IntroduceContestant:: @ 81A5323
	special BufferContestTrainerAndMonNames
	addvar VAR_0x8006, 1
	buffernumberstring 1, VAR_0x8006
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_PauseBeforeEntrance
	waitmovement 0
	releaseall
	addobject 13
	playse SE_LEDGE
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_PauseBeforeEntrance
	waitmovement 0
	releaseall
	addvar VAR_0x8006, -1
	playse SE_BALL_OPEN
	special ShowContestEntryMonPic
	call LinkContestRoom1_EventScript_AnnounceContestant
	return

LinkContestRoom1_EventScript_AnnounceContestant:: @ 81A535E
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnounceContestantOverLink
	message LinkContestRoom1_Text_IntroduceContestant
	waitmessage
	return

LinkContestRoom1_EventScript_AnnounceContestantOverLink:: @ 81A5370
	messageautoscroll LinkContestRoom1_Text_IntroduceContestant
	waitmessage
	return

LinkContestRoom1_EventScript_BeginPrimaryJudging:: @ 81A5377
	call LinkContestRoom1_EventScript_AnnouncePrimaryJudging
	call LinkContestRoom1_EventScript_AnnounceVoting
	playse SE_M_ENCORE2
	waitmessage
	call LinkContestRoom1_EventScript_CheerForWinner
	applymovement 1, LinkContestRoom1_Movement_FaceRightInPlace
	waitmovement 0
	applymovement 2, LinkContestRoom1_Movement_FaceLeftInPlace
	waitmovement 0
	delay 20
	applymovement 1, LinkContestRoom1_Movement_FaceDownInPlace
	applymovement 2, LinkContestRoom1_Movement_FaceDownInPlace
	waitmovement 0
	return

LinkContestRoom1_EventScript_AnnouncePrimaryJudging:: @ 81A53B3
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnouncePrimaryJudgingOverLink
	msgbox LinkContestRoom1_Text_BeginPrimaryJudging, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_AnnouncePrimaryJudgingOverLink:: @ 81A53C7
	messageautoscroll LinkContestRoom1_Text_BeginPrimaryJudging
	waitmessage
	return

LinkContestRoom1_EventScript_AnnounceVoting:: @ 81A53CE
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnounceVotingOverLink
	message LinkContestRoom1_Text_VotingUnderWay
	return

LinkContestRoom1_EventScript_AnnounceVotingOverLink:: @ 81A53DF
	messageautoscroll LinkContestRoom1_Text_VotingUnderWay
	return

LinkContestRoom1_EventScript_ShowContestantToAudience:: @ 81A53E5
	applymovement 1, LinkContestRoom1_Movement_FaceLeftAndReset
	waitmovement 0
	playse SE_M_ENCORE2
	call LinkContestRoom1_EventScript_TurnAudienceTowardStage
	applymovement 9, LinkContestRoom1_Movement_FaceRightAndReset
	applymovement 12, LinkContestRoom1_Movement_FaceDownAndReset
	applymovement 7, LinkContestRoom1_Movement_FaceRightAndReset
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_FaceRightAndReset
	waitmovement 0
	applymovement 10, LinkContestRoom1_Movement_FaceLeftAndReset
	applymovement 11, LinkContestRoom1_Movement_FaceDownAndReset
	applymovement 6, LinkContestRoom1_Movement_FaceUpAndReset
	applymovement 8, LinkContestRoom1_Movement_FaceLeftAndReset
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_HostTurnRight
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_HostTurnDown
	applymovement 2, LinkContestRoom1_Movement_JudgeTurnLeft
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_SetAudienceReaction:: @ 81A5455
	special GetContestMonCondition
	compare VAR_CONTEST_LOCATION, 1
	call_if_eq LinkContestRoom1_EventScript_SetNormalRankAudienceReaction
	compare VAR_CONTEST_LOCATION, 2
	call_if_eq LinkContestRoom1_EventScript_SetSuperRankAudienceReaction
	compare VAR_CONTEST_LOCATION, 3
	call_if_eq LinkContestRoom1_EventScript_SetHyperRankAudienceReaction
	compare VAR_CONTEST_LOCATION, 4
	call_if_eq LinkContestRoom1_EventScript_SetMasterRankAudienceReaction
	compare VAR_CONTEST_LOCATION, 5
	call_if_eq LinkContestRoom1_EventScript_SetLinkContestAudienceReaction
	setvar VAR_TEMP_1, 9
	setvar VAR_TEMP_2, 9
	setvar VAR_TEMP_3, 9
	setvar VAR_TEMP_4, 9
	setvar VAR_TEMP_5, 9
	setvar VAR_TEMP_6, 9
	setvar VAR_TEMP_7, 9
	setvar VAR_TEMP_8, 9
	compare VAR_TEMP_0, 0
	call_if_gt LinkContestRoom1_EventScript_AnimateAudienceHearts
	setvar VAR_TEMP_1, 0
	setvar VAR_TEMP_2, 0
	setvar VAR_TEMP_3, 0
	setvar VAR_TEMP_4, 0
	setvar VAR_TEMP_5, 0
	setvar VAR_TEMP_6, 0
	setvar VAR_TEMP_7, 0
	setvar VAR_TEMP_8, 0
	return

LinkContestRoom1_EventScript_AnimateAudienceHearts:: @ 81A54EB
	setvar VAR_RESULT, 8
	special GenerateContestRand
	compare VAR_RESULT, 0
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceOne
	compare VAR_RESULT, 1
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceTwo
	compare VAR_RESULT, 2
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceThree
	compare VAR_RESULT, 3
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceFour
	compare VAR_RESULT, 4
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceFive
	compare VAR_RESULT, 5
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceSix
	compare VAR_RESULT, 6
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceSeven
	compare VAR_RESULT, 7
	call_if_eq LinkContestRoom1_EventScript_CheerAudienceEight
	compare VAR_TEMP_0, 0
	goto_if_gt LinkContestRoom1_EventScript_AnimateAudienceHearts
	waitmovement 0
	return

LinkContestRoom1_EventScript_SetNormalRankAudienceReaction:: @ 81A555A
	compare VAR_0x8004, 80
	goto_if_gt LinkContestRoom1_EventScript_SetEightAudienceHearts
	compare VAR_0x8004, 70
	goto_if_gt LinkContestRoom1_EventScript_SetSevenAudienceHearts
	compare VAR_0x8004, 60
	goto_if_gt LinkContestRoom1_EventScript_SetSixAudienceHearts
	compare VAR_0x8004, 50
	goto_if_gt LinkContestRoom1_EventScript_SetFiveAudienceHearts
	compare VAR_0x8004, 40
	goto_if_gt LinkContestRoom1_EventScript_SetFourAudienceHearts
	compare VAR_0x8004, 30
	goto_if_gt LinkContestRoom1_EventScript_SetThreeAudienceHearts
	compare VAR_0x8004, 20
	goto_if_gt LinkContestRoom1_EventScript_SetTwoAudienceHearts
	compare VAR_0x8004, 10
	goto_if_gt LinkContestRoom1_EventScript_SetOneAudienceHeart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_SetSuperRankAudienceReaction:: @ 81A55B8
	compare VAR_0x8004, 230
	goto_if_gt LinkContestRoom1_EventScript_SetEightAudienceHearts
	compare VAR_0x8004, 210
	goto_if_gt LinkContestRoom1_EventScript_SetSevenAudienceHearts
	compare VAR_0x8004, 190
	goto_if_gt LinkContestRoom1_EventScript_SetSixAudienceHearts
	compare VAR_0x8004, 170
	goto_if_gt LinkContestRoom1_EventScript_SetFiveAudienceHearts
	compare VAR_0x8004, 150
	goto_if_gt LinkContestRoom1_EventScript_SetFourAudienceHearts
	compare VAR_0x8004, 130
	goto_if_gt LinkContestRoom1_EventScript_SetThreeAudienceHearts
	compare VAR_0x8004, 110
	goto_if_gt LinkContestRoom1_EventScript_SetTwoAudienceHearts
	compare VAR_0x8004, 90
	goto_if_gt LinkContestRoom1_EventScript_SetOneAudienceHeart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_SetHyperRankAudienceReaction:: @ 81A5616
	compare VAR_0x8004, 380
	goto_if_gt LinkContestRoom1_EventScript_SetEightAudienceHearts
	compare VAR_0x8004, 350
	goto_if_gt LinkContestRoom1_EventScript_SetSevenAudienceHearts
	compare VAR_0x8004, 320
	goto_if_gt LinkContestRoom1_EventScript_SetSixAudienceHearts
	compare VAR_0x8004, 290
	goto_if_gt LinkContestRoom1_EventScript_SetFiveAudienceHearts
	compare VAR_0x8004, 260
	goto_if_gt LinkContestRoom1_EventScript_SetFourAudienceHearts
	compare VAR_0x8004, 230
	goto_if_gt LinkContestRoom1_EventScript_SetThreeAudienceHearts
	compare VAR_0x8004, 200
	goto_if_gt LinkContestRoom1_EventScript_SetTwoAudienceHearts
	compare VAR_0x8004, 170
	goto_if_gt LinkContestRoom1_EventScript_SetOneAudienceHeart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_SetMasterRankAudienceReaction:: @ 81A5674
	compare VAR_0x8004, 600
	goto_if_gt LinkContestRoom1_EventScript_SetEightAudienceHearts
	compare VAR_0x8004, 560
	goto_if_gt LinkContestRoom1_EventScript_SetSevenAudienceHearts
	compare VAR_0x8004, 520
	goto_if_gt LinkContestRoom1_EventScript_SetSixAudienceHearts
	compare VAR_0x8004, 480
	goto_if_gt LinkContestRoom1_EventScript_SetFiveAudienceHearts
	compare VAR_0x8004, 440
	goto_if_gt LinkContestRoom1_EventScript_SetFourAudienceHearts
	compare VAR_0x8004, 400
	goto_if_gt LinkContestRoom1_EventScript_SetThreeAudienceHearts
	compare VAR_0x8004, 360
	goto_if_gt LinkContestRoom1_EventScript_SetTwoAudienceHearts
	compare VAR_0x8004, 320
	goto_if_gt LinkContestRoom1_EventScript_SetOneAudienceHeart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_SetLinkContestAudienceReaction:: @ 81A56D2
	compare VAR_0x8004, 600
	goto_if_gt LinkContestRoom1_EventScript_SetEightAudienceHearts
	compare VAR_0x8004, 550
	goto_if_gt LinkContestRoom1_EventScript_SetSevenAudienceHearts
	compare VAR_0x8004, 500
	goto_if_gt LinkContestRoom1_EventScript_SetSixAudienceHearts
	compare VAR_0x8004, 450
	goto_if_gt LinkContestRoom1_EventScript_SetFiveAudienceHearts
	compare VAR_0x8004, 400
	goto_if_gt LinkContestRoom1_EventScript_SetFourAudienceHearts
	compare VAR_0x8004, 300
	goto_if_gt LinkContestRoom1_EventScript_SetThreeAudienceHearts
	compare VAR_0x8004, 200
	goto_if_gt LinkContestRoom1_EventScript_SetTwoAudienceHearts
	compare VAR_0x8004, 100
	goto_if_gt LinkContestRoom1_EventScript_SetOneAudienceHeart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_SetOneAudienceHeart:: @ 81A5730
	setvar VAR_TEMP_0, 1
	return

LinkContestRoom1_EventScript_SetTwoAudienceHearts:: @ 81A5736
	setvar VAR_TEMP_0, 2
	return

LinkContestRoom1_EventScript_SetThreeAudienceHearts:: @ 81A573C
	setvar VAR_TEMP_0, 3
	return

LinkContestRoom1_EventScript_SetFourAudienceHearts:: @ 81A5742
	setvar VAR_TEMP_0, 4
	return

LinkContestRoom1_EventScript_SetFiveAudienceHearts:: @ 81A5748
	setvar VAR_TEMP_0, 5
	return

LinkContestRoom1_EventScript_SetSixAudienceHearts:: @ 81A574E
	setvar VAR_TEMP_0, 6
	return

LinkContestRoom1_EventScript_SetSevenAudienceHearts:: @ 81A5754
	setvar VAR_TEMP_0, 7
	return

LinkContestRoom1_EventScript_SetEightAudienceHearts:: @ 81A575A
	setvar VAR_TEMP_0, 8
	return

LinkContestRoom1_EventScript_CheerAudienceOne:: @ 81A5760
	compare VAR_TEMP_1, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceOneDone
	applymovement 6, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_1, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceOneDone:: @ 81A5783
	return

LinkContestRoom1_EventScript_CheerAudienceTwo:: @ 81A5784
	compare VAR_TEMP_2, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceTwoDone
	applymovement 12, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_2, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceTwoDone:: @ 81A57A7
	return

LinkContestRoom1_EventScript_CheerAudienceThree:: @ 81A57A8
	compare VAR_TEMP_3, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceThreeDone
	applymovement 7, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_3, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceThreeDone:: @ 81A57CB
	return

LinkContestRoom1_EventScript_CheerAudienceFour:: @ 81A57CC
	compare VAR_TEMP_4, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceFourDone
	applymovement 8, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_4, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceFourDone:: @ 81A57EF
	return

LinkContestRoom1_EventScript_CheerAudienceFive:: @ 81A57F0
	compare VAR_TEMP_5, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceFiveDone
	applymovement 9, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_5, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceFiveDone:: @ 81A5813
	return

LinkContestRoom1_EventScript_CheerAudienceSix:: @ 81A5814
	compare VAR_TEMP_6, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceSixDone
	applymovement 10, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_6, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceSixDone:: @ 81A5837
	return

LinkContestRoom1_EventScript_CheerAudienceSeven:: @ 81A5838
	compare VAR_TEMP_7, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceSevenDone
	applymovement 11, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_7, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceSevenDone:: @ 81A585B
	return

LinkContestRoom1_EventScript_CheerAudienceEight:: @ 81A585C
	compare VAR_TEMP_8, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceEightDone
	applymovement 15, LinkContestRoom1_Movement_CheerWithHeart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_8, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceEightDone:: @ 81A587F
	return

LinkContestRoom1_EventScript_ReturnContestantToPlace:: @ 81A5880
	closemessage
	release
	removeobject 13
	special HideContestEntryMonPic
	switch VAR_0x8006
	case 0, LinkContestRoom1_EventScript_ReturnContestantOne
	case 1, LinkContestRoom1_EventScript_ReturnContestantTwo
	case 2, LinkContestRoom1_EventScript_ReturnContestantThree
	case 3, LinkContestRoom1_EventScript_ReturnContestantFour
	return

LinkContestRoom1_EventScript_ReturnContestantOne:: @ 81A58BA
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_ContestantOneExit
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_ReturnContestantTwo:: @ 81A58C7
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_ContestantTwoExit
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_ReturnContestantThree:: @ 81A58D4
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_ContestantThreeExit
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_ReturnContestantFour:: @ 81A58E1
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_ContestantFourExit
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_StartSecondaryJudging:: @ 81A58EE
	lockall
	applymovement 1, LinkContestRoom1_Movement_FaceAudience
	waitmovement 0
	call LinkContestRoom1_EventScript_AnnounceSecondaryJudging
	waitmessage
	applymovement 1, LinkContestRoom1_Movement_WalkLeftFromStage
	applymovement 2, LinkContestRoom1_Movement_WalkRightFromStage
	waitmovement 0
	releaseall
	setvar VAR_TEMP_9, 1
	startcontest
	setvar VAR_TEMP_9, 0
	lockall
	applymovement 1, LinkContestRoom1_Movement_WalkRightFromStage
	applymovement 2, LinkContestRoom1_Movement_WalkLeftFromStage
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_AnnounceSecondaryJudging:: @ 81A5930
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnounceSecondaryJudgingOverLink
	msgbox LinkContestRoom1_Text_BeginSecondaryJudging, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_AnnounceSecondaryJudgingOverLink:: @ 81A5944
	messageautoscroll LinkContestRoom1_Text_BeginSecondaryJudging
	return

LinkContestRoom1_EventScript_AnnounceJudgingResults:: @ 81A594A
	call LinkContestRoom1_EventScript_AnnounceEndOfJudging
	call LinkContestRoom1_EventScript_ThankContestants
	applymovement 1, LinkContestRoom1_Movement_FaceRight
	waitmovement 0
	call LinkContestRoom1_EventScript_AnnounceWinnerSoon
	call LinkContestRoom1_EventScript_JudgeAnnouncesWinner
	applymovement 1, LinkContestRoom1_Movement_FaceDownInPlace
	waitmovement 0
	closemessage
	releaseall
	setvar VAR_TEMP_9, 1
	showcontestresults
	setvar VAR_TEMP_9, 0
	playbgm MUS_CONTEST_WINNER, FALSE
	return

LinkContestRoom1_EventScript_AnnounceEndOfJudging:: @ 81A5984
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnounceEndOfJudgingOverLink
	msgbox LinkContestRoom1_Text_JudgingFinished, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_AnnounceEndOfJudgingOverLink:: @ 81A5998
	messageautoscroll LinkContestRoom1_Text_JudgingFinished
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_ThankContestants:: @ 81A59A2
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_ThankContestantsOverLink
	msgbox LinkContestRoom1_Text_ThankContestants, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_ThankContestantsOverLink:: @ 81A59B6
	messageautoscroll LinkContestRoom1_Text_ThankContestants
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_AnnounceWinnerSoon:: @ 81A59C0
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnounceWinnerSoonOverLink
	msgbox LinkContestRoom1_Text_WinnerAboutToBeAnnounced, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_AnnounceWinnerSoonOverLink:: @ 81A59D4
	messageautoscroll LinkContestRoom1_Text_WinnerAboutToBeAnnounced
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_JudgeAnnouncesWinner:: @ 81A59DE
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_JudgeAnnouncesWinnerOverLink
	msgbox LinkContestRoom1_Text_JudgeDeclareWinner, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_JudgeAnnouncesWinnerOverLink:: @ 81A59F2
	messageautoscroll LinkContestRoom1_Text_JudgeDeclareWinner
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_SetWinnerObject:: @ 81A59FC
	special GetContestWinnerId
	switch VAR_0x8005
	case 0, LinkContestRoom1_EventScript_SetWinnerObjectOne
	case 1, LinkContestRoom1_EventScript_SetWinnerObjectTwo
	case 2, LinkContestRoom1_EventScript_SetWinnerObjectThree
	case 3, LinkContestRoom1_EventScript_SetWinnerObjectFour
	return

LinkContestRoom1_EventScript_SetWinnerObjectOne:: @ 81A5A31
	setvar VAR_TEMP_3, 3
	return

LinkContestRoom1_EventScript_SetWinnerObjectTwo:: @ 81A5A37
	setvar VAR_TEMP_3, 4
	return

LinkContestRoom1_EventScript_SetWinnerObjectThree:: @ 81A5A3D
	setvar VAR_TEMP_3, 5
	return

LinkContestRoom1_EventScript_SetWinnerObjectFour:: @ 81A5A43
	setvar VAR_TEMP_3, 14
	return

LinkContestRoom1_EventScript_AnnounceWinner:: @ 81A5A49
	special BufferContestWinnerTrainerName
	special BufferContestWinnerMonName
	addvar VAR_0x8005, 1
	buffernumberstring 1, VAR_0x8005
	addvar VAR_0x8005, -1
	call LinkContestRoom1_EventScript_AnnounceWinnerAtHall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_WalkUpAndFaceDown
	waitmovement 0
	playse SE_M_ENCORE2
	setvar VAR_TEMP_1, 0
	return

LinkContestRoom1_EventScript_AnnounceWinnerAtHall:: @ 81A5A75
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AnnounceWinnerOverLink
	msgbox LinkContestRoom1_Text_CongratulateWinner, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_AnnounceWinnerOverLink:: @ 81A5A89
	messageautoscroll LinkContestRoom1_Text_CongratulateWinner
	waitmessage
	return

LinkContestRoom1_EventScript_CheerForWinner:: @ 81A5A90
	addvar VAR_TEMP_1, 1
	lockall
	compare VAR_CONTEST_LOCATION, 1
	call_if_gt LinkContestRoom1_EventScript_TurnAudienceTowardStage
	applymovement 9, LinkContestRoom1_Movement_FaceRightAndReset
	applymovement 12, LinkContestRoom1_Movement_FaceDownAndReset
	applymovement 7, LinkContestRoom1_Movement_FaceRightAndReset
	delay 30
	applymovement 10, LinkContestRoom1_Movement_FaceLeftAndReset
	applymovement 11, LinkContestRoom1_Movement_FaceDownAndReset
	applymovement 6, LinkContestRoom1_Movement_FaceUpAndReset
	applymovement 8, LinkContestRoom1_Movement_FaceLeftAndReset
	compare VAR_TEMP_1, 4
	goto_if_ne LinkContestRoom1_EventScript_CheerForWinner
	delay 30
	return

LinkContestRoom1_EventScript_TurnAudienceTowardStage:: @ 81A5AE4
	turnvobject 0, 1
	turnvobject 2, 1
	turnvobject 4, 4
	turnvobject 6, 1
	turnvobject 8, 1
	turnvobject 10, 1
	turnvobject 12, 1
	turnvobject 14, 1
	turnvobject 16, 1
	turnvobject 18, 1
	turnvobject 20, 4
	turnvobject 22, 4
	turnvobject 25, 4
	turnvobject 27, 3
	turnvobject 28, 4
	delay 10
	turnvobject 0, 4
	turnvobject 2, 4
	turnvobject 4, 4
	turnvobject 6, 4
	turnvobject 8, 4
	turnvobject 10, 3
	turnvobject 12, 3
	turnvobject 14, 3
	turnvobject 16, 3
	turnvobject 18, 3
	turnvobject 20, 1
	turnvobject 22, 1
	turnvobject 25, 2
	turnvobject 27, 2
	turnvobject 28, 2
	delay 10
	turnvobject 1, 2
	turnvobject 3, 2
	turnvobject 5, 2
	turnvobject 7, 2
	turnvobject 9, 4
	turnvobject 11, 2
	turnvobject 15, 2
	turnvobject 13, 2
	turnvobject 17, 2
	turnvobject 19, 2
	turnvobject 21, 3
	turnvobject 23, 3
	turnvobject 24, 3
	turnvobject 26, 4
	turnvobject 29, 3
	turnvobject 30, 3
	delay 10
	turnvobject 1, 4
	turnvobject 3, 4
	turnvobject 5, 4
	turnvobject 7, 4
	turnvobject 9, 4
	turnvobject 11, 3
	turnvobject 15, 3
	turnvobject 13, 3
	turnvobject 17, 3
	turnvobject 19, 3
	turnvobject 21, 1
	turnvobject 23, 1
	turnvobject 24, 1
	turnvobject 26, 2
	turnvobject 29, 2
	turnvobject 30, 2
	delay 10
	return

LinkContestRoom1_EventScript_AwardContestWinner:: @ 81A5BAB
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_FinishLinkContest
	call LinkContestRoom1_EventScript_CheckPreviousContestWin
	goto_if_set FLAG_TEMP_2, LinkContestRoom1_EventScript_FinishContestWithoutPrize
	msgbox LinkContestRoom1_Text_PleaseAcceptPrize, MSGBOX_SIGN
	waitmessage
	call LinkContestRoom1_EventScript_MoveWinnerToStage
	call LinkContestRoom1_EventScript_CheckWinnerPrize
	playse SE_M_ENCORE2
	setvar VAR_TEMP_1, 0
	call LinkContestRoom1_EventScript_CheerForWinner
	delay 30
	special ShouldReadyContestArtist
	compare VAR_0x8004, 1
	goto_if_eq LinkContestRoom1_EventScript_ReadyContestArtist
	return

LinkContestRoom1_EventScript_FinishContestWithoutPrize:: @ 81A5BF6
	incrementgamestat GAME_STAT_WON_CONTEST
	msgbox LinkContestRoom1_Text_CongratulationsCompeteAgain, MSGBOX_SIGN
	delay 90
	special ShouldReadyContestArtist
	compare VAR_0x8004, 1
	goto_if_eq LinkContestRoom1_EventScript_ReadyContestArtist
	return

LinkContestRoom1_EventScript_CheckPreviousContestWin:: @ 81A5C12
	specialvar VAR_RESULT, HasMonWonThisContestBefore
	compare VAR_RESULT, 1
	goto_if_eq LinkContestRoom1_EventScript_CheckPlayerIsWinner
	return

LinkContestRoom1_EventScript_CheckPlayerIsWinner:: @ 81A5C23
	special GetContestWinnerId
	compare VAR_0x8005, 3
	goto_if_eq LinkContestRoom1_EventScript_CheckMasterRankPrize
	return

LinkContestRoom1_EventScript_CheckMasterRankPrize:: @ 81A5C32
	compare VAR_CONTEST_RANK, 3
	goto_if_eq LinkContestRoom1_EventScript_ReturnAfterPrizeCheck
	setflag FLAG_TEMP_2
	return

LinkContestRoom1_EventScript_ReturnAfterPrizeCheck:: @ 81A5C41
	return

LinkContestRoom1_EventScript_MarkContestSketchCreated:: @ 81A5C42
	setflag FLAG_CONTEST_SKETCH_CREATED
	return

LinkContestRoom1_EventScript_ReadyContestArtist:: @ 81A5C46
	setvar VAR_LILYCOVE_CONTEST_LOBBY_STATE, LILYCOVE_CONTEST_LOBBY_STATE_CONTEST_ARTIST
	return

LinkContestRoom1_EventScript_FinishLinkContest:: @ 81A5C4C
	delay 60
	call LinkContestRoom1_EventScript_RecordLinkContestWin
	special ShouldReadyContestArtist
	compare VAR_0x8004, 1
	goto_if_eq LinkContestRoom1_EventScript_ReadyLinkContestArtist
	closemessage
	return

LinkContestRoom1_EventScript_ReadyLinkContestArtist:: @ 81A5C64
	setvar VAR_LILYCOVE_CONTEST_LOBBY_STATE, LILYCOVE_CONTEST_LOBBY_STATE_LINK_CONTEST_ARTIST
	return

LinkContestRoom1_EventScript_RecordLinkContestWin:: @ 81A5C6A
	special GetContestPlayerId
	special GetContestWinnerId
	compare VAR_0x8004, VAR_0x8005
	goto_if_eq LinkContestRoom1_EventScript_IncrementLinkContestWins
	return

LinkContestRoom1_EventScript_IncrementLinkContestWins:: @ 81A5C7C
	incrementgamestat GAME_STAT_WON_LINK_CONTEST
	return

LinkContestRoom1_EventScript_MoveWinnerToStage:: @ 81A5C7F
	switch VAR_0x8005
	case 0, LinkContestRoom1_EventScript_MoveWinnerOneToStage
	case 1, LinkContestRoom1_EventScript_MoveWinnerTwoToStage
	case 2, LinkContestRoom1_EventScript_MoveWinnerThreeToStage
	case 3, LinkContestRoom1_EventScript_MoveWinnerFourToStage
	return

LinkContestRoom1_EventScript_MoveWinnerOneToStage:: @ 81A5CB1
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_WinnerOneEnter
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_MoveWinnerTwoToStage:: @ 81A5CBE
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_WinnerTwoEnter
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_MoveWinnerThreeToStage:: @ 81A5CCB
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_WinnerOneToStage
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_MoveWinnerFourToStage:: @ 81A5CD8
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_WinnerTwoToStage
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_CheckWinnerPrize:: @ 81A5CE5
	special GetContestWinnerId
	compare VAR_0x8005, 3
	goto_if_eq LinkContestRoom1_EventScript_AwardPlayerContestWin
	msgbox LinkContestRoom1_Text_CongratulationsCompeteAgain, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_AwardPlayerContestWin:: @ 81A5CFC
	compare VAR_CONTEST_LOCATION, 2
	call_if_eq LinkContestRoom1_EventScript_MarkContestSketchCreated
	incrementgamestat GAME_STAT_WON_CONTEST
	specialvar VAR_RESULT, HasMonWonThisContestBefore
	compare VAR_RESULT, 0
	goto_if_eq LinkContestRoom1_EventScript_GiveContestRibbon
	compare VAR_CONTEST_RANK, 3
	goto_if_eq LinkContestRoom1_EventScript_GiveLuxuryBallPrize
	msgbox LinkContestRoom1_Text_CongratulationsCompeteAgain, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_DeferContestPrizePickup:: @ 81A5D2D
	msgbox LinkContestRoom1_Text_PickUpPrizeAtReception, MSGBOX_SIGN
	setvar VAR_CONTEST_PRIZE_PICKUP, 4
	return

LinkContestRoom1_EventScript_GiveLuxuryBallPrize:: @ 81A5D3B
	giveitem ITEM_LUXURY_BALL
	compare VAR_RESULT, 0
	goto_if_eq LinkContestRoom1_EventScript_DeferContestPrizePickup
	msgbox LinkContestRoom1_Text_CongratulationsCompeteAgain, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_GiveContestRibbon:: @ 81A5D5B
	special GiveMonContestRibbon
	incrementgamestat GAME_STAT_RECEIVED_RIBBONS
	setflag FLAG_SYS_RIBBON_GET
	lockall
	msgbox LinkContestRoom1_Text_AwardRibbon, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	msgbox LinkContestRoom1_Text_ReceivedRibbon, MSGBOX_DEFAULT
	waitfanfare
	special BufferContestWinnerMonName
	msgbox LinkContestRoom1_Text_PutRibbonOnMon, MSGBOX_DEFAULT
	releaseall
	return

LinkContestRoom1_Movement_FaceDown:: @ 81A5D85
	walk_down
	step_end

LinkContestRoom1_Movement_WalkUpFacingLocked:: @ 81A5D87
	lock_facing_direction
	walk_up
	unlock_facing_direction
	step_end

LinkContestRoom1_Movement_FaceRight:: @ 81A5D8B
	walk_in_place_faster_right
	step_end

LinkContestRoom1_Movement_CheerWithHeart:: @ 81A5D8D
	emote_heart
	step_end

LinkContestRoom1_Movement_FaceDownInPlace:: @ 81A5D8F
	walk_in_place_faster_down
	step_end

LinkContestRoom1_Movement_WalkLeftFromStage:: @ 81A5D91
	walk_left
	walk_left
	walk_in_place_faster_down
	step_end

LinkContestRoom1_Movement_WalkRightFromStage:: @ 81A5D95
	walk_right
	walk_right
	walk_in_place_faster_down
	step_end

LinkContestRoom1_Movement_WalkUpAndFaceDown:: @ 81A5D99
	walk_up
	walk_in_place_faster_down
	step_end

LinkContestRoom1_Movement_PauseBeforeEntrance:: @ 81A5D9C
	delay_16
	delay_16
	step_end

LinkContestRoom1_Movement_FaceRightInPlace:: @ 81A5D9F
	walk_in_place_faster_right
	step_end

LinkContestRoom1_Movement_FaceLeftInPlace:: @ 81A5DA1
	walk_in_place_faster_left
	step_end

LinkContestRoom1_Movement_FaceAudience:: @ 81A5DA3
	walk_in_place_faster_down
	step_end

LinkContestRoom1_Movement_WinnerOneToStage:: @ 81A5DA5
	walk_left
	walk_left
	walk_up
	step_end

LinkContestRoom1_Movement_WinnerTwoToStage:: @ 81A5DA9
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	step_end

LinkContestRoom1_Movement_FaceLeftAndReset:: @ 81A5DAF
	face_left
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_FaceUpAndReset:: @ 81A5DB3
	face_up
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_FaceRightAndReset:: @ 81A5DB7
	face_right
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_FaceDownAndReset:: @ 81A5DBB
	face_down
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_HostFaceUp:: @ 81A5DBF
	face_up
	step_end

LinkContestRoom1_Movement_HostTurnRight:: @ 81A5DC1
	face_up
	delay_16
	walk_in_place_faster_right
	step_end

LinkContestRoom1_Movement_JudgeTurnLeft:: @ 81A5DC5
	walk_in_place_faster_left
	delay_16
	delay_16
	walk_in_place_faster_down
	step_end

LinkContestRoom1_Movement_HostTurnDown:: @ 81A5DCA
	delay_16
	delay_16
	walk_in_place_faster_down
	step_end

LinkContestRoom1_Movement_ContestantOneEnter:: @ 81A5DCE
	walk_up
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_ContestantOneExit:: @ 81A5DD4
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_down
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_ContestantTwoEnter:: @ 81A5DDA
	walk_up
	walk_right
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_ContestantTwoExit:: @ 81A5DDE
	walk_fast_left
	walk_fast_down
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_ContestantThreeEnter:: @ 81A5DE2
	walk_up
	walk_left
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_ContestantThreeExit:: @ 81A5DE6
	walk_fast_right
	walk_fast_down
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_ContestantFourEnter:: @ 81A5DEA
	walk_up
	walk_left
	walk_left
	walk_left
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_ContestantFourExit:: @ 81A5DF0
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_in_place_faster_up
	step_end

LinkContestRoom1_Movement_WinnerOneEnter:: @ 81A5DF6
	walk_right
	walk_right
	walk_up
	step_end

LinkContestRoom1_Movement_WinnerTwoEnter:: @ 81A5DFA
	walk_up
	step_end
