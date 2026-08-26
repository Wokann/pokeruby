LilycoveCity_ContestLobby_EventScript_SpeakToContestReceptionist:: @ 81A4E92
	lock
	faceplayer
	checkitem ITEM_CONTEST_PASS, 1
	compare VAR_RESULT, 0
	call_if_eq LilycoveCity_ContestLobby_EventScript_ClearContestReceptionState
	compare VAR_CONTEST_PRIZE_PICKUP, 0
	goto_if_ne LilycoveCity_ContestLobby_EventScript_PickUpPrize
	goto_if_set FLAG_TEMP_1, LilycoveCity_ContestLobby_EventScript_AskEnterContest
	bufferstdstring 0, VAR_0x800B
	msgbox LilycoveCity_ContestLobby_Text_ContestReception, MSGBOX_DEFAULT
	checkitem ITEM_CONTEST_PASS, 1
	compare VAR_RESULT, 0
	goto_if_eq LilycoveCity_ContestLobby_EventScript_CheckContestPass
	setflag FLAG_TEMP_1
	goto LilycoveCity_ContestLobby_EventScript_AskEnterContest
	end

LilycoveCity_ContestLobby_EventScript_ClearContestReceptionState:: @ 81A4EDD
	clearflag FLAG_TEMP_1
	return

LilycoveCity_ContestLobby_EventScript_CheckContestPass:: @ 81A4EE1
	compare VAR_CONTEST_RANK, 0
	goto_if_eq LilycoveCity_ContestLobby_EventScript_OfferContestPass
	msgbox LilycoveCity_ContestLobby_Text_NeedContestPassForNonNormalRank, MSGBOX_DEFAULT
	releaseall
	end

LilycoveCity_ContestLobby_EventScript_OfferContestPass:: @ 81A4EF6
	msgbox LilycoveCity_ContestLobby_Text_OfferContestPass, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq LilycoveCity_ContestLobby_EventScript_GiveContestPass
	msgbox LilycoveCity_ContestLobby_Text_ComeBackToEnterContest, MSGBOX_DEFAULT
	releaseall
	end

LilycoveCity_ContestLobby_EventScript_GiveContestPass:: @ 81A4F13
	goto_if_set FLAG_RECEIVED_CONTEST_PASS, LilycoveCity_ContestLobby_EventScript_AlreadyReceivedContestPass
	msgbox LilycoveCity_ContestLobby_Text_ContestPassIssued, MSGBOX_DEFAULT
	giveitem ITEM_CONTEST_PASS
	setflag FLAG_RECEIVED_CONTEST_PASS
	setflag FLAG_TEMP_1
	msgbox LilycoveCity_ContestLobby_Text_ExplainContests, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_AskEnterContest
	end

LilycoveCity_ContestLobby_EventScript_AlreadyReceivedContestPass:: @ 81A4F44
	msgbox LilycoveCity_ContestLobby_Text_AlreadyReceivedContestPass, MSGBOX_DEFAULT
	releaseall
	end

LilycoveCity_ContestLobby_EventScript_PickUpPrize:: @ 81A4F4E
	msgbox LilycoveCity_ContestLobby_Text_PokemonWonWeHavePrize, MSGBOX_DEFAULT
	switch VAR_CONTEST_PRIZE_PICKUP
	case 4, LilycoveCity_ContestLobby_EventScript_GiveLuxuryBallAtCounter
	end

LilycoveCity_ContestLobby_EventScript_GiveLuxuryBallAtCounter:: @ 81A4F67
	giveitem ITEM_LUXURY_BALL
	compare VAR_RESULT, 0
	goto_if_eq LilycoveCity_ContestLobby_EventScript_NoRoomForLuxuryBallAtCounter
	setvar VAR_CONTEST_PRIZE_PICKUP, 0
	closemessage
	release
	end

LilycoveCity_ContestLobby_EventScript_NoRoomForLuxuryBallAtCounter:: @ 81A4F86
	msgbox LilycoveCity_ContestLobby_Text_ComeBackForPrizeLater, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_AskEnterContest:: @ 81A4F8F
	message LilycoveCity_ContestLobby_Text_EnterContest1
	waitmessage
	multichoice 0, 0, 2, 0
	switch VAR_RESULT
	case 0, LilycoveCity_ContestLobby_EventScript_ChooseContestType
	case 1, LilycoveCity_ContestLobby_EventScript_ContestInfo
	case 2, LilycoveCity_ContestLobby_EventScript_CancelEnterContest
	case 127, LilycoveCity_ContestLobby_EventScript_CancelEnterContest
	end

LilycoveCity_ContestLobby_EventScript_ContestInfo:: @ 81A4FCC
	message LilycoveCity_ContestLobby_Text_WhichTopic1
	waitmessage
	multichoice 0, 0, 3, 0
	switch VAR_RESULT
	case 0, LilycoveCity_ContestLobby_EventScript_ExplainContests
	case 1, LilycoveCity_ContestLobby_EventScript_ExplainContestTypes
	case 2, LilycoveCity_ContestLobby_EventScript_ExplainContestRanks
	case 3, LilycoveCity_ContestLobby_EventScript_AskEnterContest
	case 127, LilycoveCity_ContestLobby_EventScript_AskEnterContest
	end

LilycoveCity_ContestLobby_EventScript_ExplainContests:: @ 81A5014
	msgbox LilycoveCity_ContestLobby_Text_ExplainContests, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_ContestInfo
	end

LilycoveCity_ContestLobby_EventScript_ExplainContestTypes:: @ 81A5022
	msgbox LilycoveCity_ContestLobby_Text_ExplainContestTypes, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_ContestInfo
	end

LilycoveCity_ContestLobby_EventScript_ExplainContestRanks:: @ 81A5030
	msgbox LilycoveCity_ContestLobby_Text_ExplainContestRanks, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_ContestInfo
	end

LilycoveCity_ContestLobby_EventScript_CancelEnterContest:: @ 81A503E
	msgbox LilycoveCity_ContestLobby_Text_ParticipateAnotherTime, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_ChooseContestMon:: @ 81A5048
	msgbox LilycoveCity_ContestLobby_Text_EnterWhichPokemon1, MSGBOX_DEFAULT
	choosecontestmon
	compare VAR_0x8004, 255
	goto_if_eq LilycoveCity_ContestLobby_EventScript_ChooseContestType
	special ScrSpecial_CheckSelectedMonAndInitContest
	compare VAR_RESULT, 0
	goto_if_eq LilycoveCity_ContestLobby_EventScript_CantEnterLowRank
	compare VAR_RESULT, 1
	goto_if_eq LilycoveCity_ContestLobby_EventScript_ConfirmEntry
	compare VAR_RESULT, 2
	goto_if_eq LilycoveCity_ContestLobby_EventScript_ConfirmEntryAlreadyWon
	compare VAR_RESULT, 3
	goto_if_eq LilycoveCity_ContestLobby_EventScript_CantEnterEgg
	compare VAR_RESULT, 4
	goto_if_eq LilycoveCity_ContestLobby_EventScript_CantEnterFainted
	end

LilycoveCity_ContestLobby_EventScript_ChooseContestType:: @ 81A5097
	message LilycoveCity_ContestLobby_Text_EnterWhichContest1
	waitmessage
	multichoice 0, 0, 4, 0
	switch VAR_RESULT
	case 5, LilycoveCity_ContestLobby_EventScript_CancelEnterContest
	case 127, LilycoveCity_ContestLobby_EventScript_CancelEnterContest
	copyvar VAR_CONTEST_CATEGORY, VAR_RESULT
	goto LilycoveCity_ContestLobby_EventScript_ChooseContestMon
	end

LilycoveCity_ContestLobby_EventScript_CantEnterLowRank:: @ 81A50C8
	msgbox LilycoveCity_ContestLobby_Text_MonNotQualifiedForRank, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_ChooseContestMon
	release
	end

LilycoveCity_ContestLobby_EventScript_ConfirmEntry:: @ 81A50D7
	msgbox LilycoveCity_ContestLobby_Text_ConfirmContestMon, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, LilycoveCity_ContestLobby_EventScript_ChooseContestMon
	case YES, LilycoveCity_ContestLobby_EventScript_EnterMon
	end

LilycoveCity_ContestLobby_EventScript_ConfirmEntryAlreadyWon:: @ 81A50FB
	msgbox LilycoveCity_ContestLobby_Text_AlreadyWonEnterAnyway, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, LilycoveCity_ContestLobby_EventScript_ChooseContestMon
	case YES, LilycoveCity_ContestLobby_EventScript_EnterMon
	end

LilycoveCity_ContestLobby_EventScript_CantEnterEgg:: @ 81A511F
	msgbox LilycoveCity_ContestLobby_Text_EggCannotTakePart, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_ChooseContestMon
	release
	end

LilycoveCity_ContestLobby_EventScript_CantEnterFainted:: @ 81A512E
	msgbox LilycoveCity_ContestLobby_Text_MonInNoConditionForContest, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_ChooseContestMon
	release
	end

LilycoveCity_ContestLobby_EventScript_EnterMon:: @ 81A513D
	msgbox LilycoveCity_ContestLobby_Text_YourMonIsEntryNum4, MSGBOX_DEFAULT
	closemessage
	releaseall
	setvar VAR_LINK_CONTEST_ROOM_STATE, 1
	return

LinkContestRoom1_EventScript_DoContest:: @ 81A514D
	setvar VAR_0x8006, 0
	lockall
	applymovement 14, LinkContestRoom1_Movement_Player4FaceUp
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_MCWalkDown
	waitmovement 0
	releaseall
	call LinkContestRoom1_EventScript_GetContestRankStringId
	call LinkContestRoom1_EventScript_GetContestCategory
	call LinkContestRoom1_EventScript_ContestGettingStarted
	call LinkContestRoom1_EventScript_ShowContestMons
	call LinkContestRoom1_EventScript_DoContestAppeals
	call LinkContestRoom1_EventScript_ContestResults
	call LinkContestRoom1_EventScript_GetWinnerObjEventId
	call LinkContestRoom1_EventScript_CongratulateWinner
	call LinkContestRoom1_EventScript_AudienceLookAround
	call LinkContestRoom1_EventScript_GiveWinnerPrize
	setvar VAR_LINK_CONTEST_ROOM_STATE, 2
	return

LinkContestRoom1_EventScript_GetContestRankStringId:: @ 81A51A0
	switch VAR_CONTEST_RANK
	case 0, LinkContestRoom1_EventScript_GetNormalStringId
	case 1, LinkContestRoom1_EventScript_GetSuperStringId
	case 2, LinkContestRoom1_EventScript_GetHyperStringId
	case 3, LinkContestRoom1_EventScript_GetMasterStringId
	return

LinkContestRoom1_EventScript_GetNormalStringId:: @ 81A51D2
	setvar VAR_0x8009, 5
	return

LinkContestRoom1_EventScript_GetSuperStringId:: @ 81A51D8
	setvar VAR_0x8009, 6
	return

LinkContestRoom1_EventScript_GetHyperStringId:: @ 81A51DE
	setvar VAR_0x8009, 7
	return

LinkContestRoom1_EventScript_GetMasterStringId:: @ 81A51E4
	setvar VAR_0x8009, 8
	return

LinkContestRoom1_EventScript_GetContestCategory:: @ 81A51EA
	switch VAR_CONTEST_CATEGORY
	case 0, LinkContestRoom1_EventScript_GetCategoryCool
	case 1, LinkContestRoom1_EventScript_GetCategoryBeauty
	case 2, LinkContestRoom1_EventScript_GetCategoryCute
	case 3, LinkContestRoom1_EventScript_GetCategorySmart
	case 4, LinkContestRoom1_EventScript_GetCategoryTough
	return

LinkContestRoom1_EventScript_GetCategoryCool:: @ 81A5227
	setvar VAR_0x8008, 0
	return

LinkContestRoom1_EventScript_GetCategoryBeauty:: @ 81A522D
	setvar VAR_0x8008, 1
	return

LinkContestRoom1_EventScript_GetCategoryCute:: @ 81A5233
	setvar VAR_0x8008, 2
	return

LinkContestRoom1_EventScript_GetCategorySmart:: @ 81A5239
	setvar VAR_0x8008, 3
	return

LinkContestRoom1_EventScript_GetCategoryTough:: @ 81A523F
	setvar VAR_0x8008, 4
	return

LinkContestRoom1_EventScript_ContestGettingStarted:: @ 81A5245
	bufferstdstring 1, VAR_0x8008
	bufferstdstring 2, VAR_0x8009
	call LinkContestRoom1_EventScript_GettingStarted
	lockall
	applymovement 1, LinkContestRoom1_Movement_MCBackUp
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_GettingStarted:: @ 81A525F
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_GettingStartedLink
	msgbox LinkContestRoom1_Text_GettingStartedParticipantsAsFollows, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_GettingStartedLink:: @ 81A5273
	messageautoscroll LinkContestRoom1_Text_GettingStartedParticipantsAsFollowsLink
	waitmessage
	return

LinkContestRoom1_EventScript_ShowContestMons:: @ 81A527A
	call LinkContestRoom1_EventScript_ContestantWalkToCenter
	call LinkContestRoom1_EventScript_ShowContestMonPic
	call LinkContestRoom1_EventScript_AudienceHeartEmotes
	call LinkContestRoom1_EventScript_AudienceReactToContestant
	call LinkContestRoom1_EventScript_ContestantReturn
	addvar VAR_0x8006, 1
	compare VAR_0x8006, 4
	goto_if_ne LinkContestRoom1_EventScript_ShowContestMons
	call LinkContestRoom1_EventScript_AudienceVote
	setvar VAR_TEMP_1, 6
	return

LinkContestRoom1_EventScript_ContestantWalkToCenter:: @ 81A52AE
	compare VAR_0x8006, 0
	goto_if_eq LinkContestRoom1_EventScript_Player1WalkToCenter
	compare VAR_0x8006, 1
	goto_if_eq LinkContestRoom1_EventScript_Player2WalkToCenter
	compare VAR_0x8006, 2
	goto_if_eq LinkContestRoom1_EventScript_Player3WalkToCenter
	compare VAR_0x8006, 3
	goto_if_eq LinkContestRoom1_EventScript_Player4WalkToCenter
	return

LinkContestRoom1_EventScript_Player1WalkToCenter:: @ 81A52DB
	lockall
	applymovement 3, LinkContestRoom1_Movement_Player1WalkToCenter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 3
	return

LinkContestRoom1_EventScript_Player2WalkToCenter:: @ 81A52ED
	lockall
	applymovement 4, LinkContestRoom1_Movement_Player2WalkToCenter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 4
	return

LinkContestRoom1_EventScript_Player3WalkToCenter:: @ 81A52FF
	lockall
	applymovement 5, LinkContestRoom1_Movement_Player3WalkToCenter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 5
	return

LinkContestRoom1_EventScript_Player4WalkToCenter:: @ 81A5311
	lockall
	applymovement 14, LinkContestRoom1_Movement_Player4WalkToCenter
	waitmovement 0
	releaseall
	setvar VAR_0x800B, 14
	return

LinkContestRoom1_EventScript_ShowContestMonPic:: @ 81A5323
	special BufferContestTrainerAndMonNames
	addvar VAR_0x8006, 1
	buffernumberstring 1, VAR_0x8006
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_ContestantDelay32
	waitmovement 0
	releaseall
	addobject 13
	playse SE_LEDGE
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_ContestantDelay32
	waitmovement 0
	releaseall
	addvar VAR_0x8006, -1
	playse SE_BALL_OPEN
	special ShowContestEntryMonPic
	call LinkContestRoom1_EventScript_EntryXTrainersMon
	return

LinkContestRoom1_EventScript_EntryXTrainersMon:: @ 81A535E
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_EntryXTrainersMonLink
	message LinkContestRoom1_Text_EntryXTrainersMon
	waitmessage
	return

LinkContestRoom1_EventScript_EntryXTrainersMonLink:: @ 81A5370
	messageautoscroll LinkContestRoom1_Text_EntryXTrainersMon
	waitmessage
	return

LinkContestRoom1_EventScript_AudienceVote:: @ 81A5377
	call LinkContestRoom1_EventScript_AudienceWillVote
	call LinkContestRoom1_EventScript_VotingUnderWay
	playse SE_M_ENCORE2
	waitmessage
	call LinkContestRoom1_EventScript_AudienceLookAround
	applymovement 1, LinkContestRoom1_Movement_MCFaceJudge2
	waitmovement 0
	applymovement 2, LinkContestRoom1_Movement_JudgeFaceMC
	waitmovement 0
	delay 20
	applymovement 1, LinkContestRoom1_Movement_FaceContestants
	applymovement 2, LinkContestRoom1_Movement_FaceContestants
	waitmovement 0
	return

LinkContestRoom1_EventScript_AudienceWillVote:: @ 81A53B3
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_AudienceWillVoteLink
	msgbox LinkContestRoom1_Text_SeenContestantsAudienceWillVote, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_AudienceWillVoteLink:: @ 81A53C7
	messageautoscroll LinkContestRoom1_Text_SeenContestantsAudienceWillVote
	waitmessage
	return

LinkContestRoom1_EventScript_VotingUnderWay:: @ 81A53CE
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_VotingUnderWayLink
	message LinkContestRoom1_Text_VotingUnderWay
	return

LinkContestRoom1_EventScript_VotingUnderWayLink:: @ 81A53DF
	messageautoscroll LinkContestRoom1_Text_VotingUnderWay
	return

LinkContestRoom1_EventScript_AudienceReactToContestant:: @ 81A53E5
	applymovement 1, LinkContestRoom1_Movement_AudienceMemberLookLeft
	waitmovement 0
	playse SE_M_ENCORE2
	call LinkContestRoom1_EventScript_VObjectAudienceLookAround
	applymovement 9, LinkContestRoom1_Movement_AudienceMemberLookRight
	applymovement 12, LinkContestRoom1_Movement_AudienceMemberLookDown
	applymovement 7, LinkContestRoom1_Movement_AudienceMemberLookRight
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_AudienceMemberLookRight
	waitmovement 0
	applymovement 10, LinkContestRoom1_Movement_AudienceMemberLookLeft
	applymovement 11, LinkContestRoom1_Movement_AudienceMemberLookDown
	applymovement 6, LinkContestRoom1_Movement_AudienceMemberLookUp
	applymovement 8, LinkContestRoom1_Movement_AudienceMemberLookLeft
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_MCLookAtJudge
	waitmovement 0
	applymovement 1, LinkContestRoom1_Movement_MCWalkInPlaceDown
	applymovement 2, LinkContestRoom1_Movement_JudgeLookAtMC
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_AudienceHeartEmotes:: @ 81A5455
	special ScrSpecial_GetMonCondition
	compare VAR_CONTEST_LOCATION, 1
	call_if_eq LinkContestRoom1_EventScript_GetNumberOfHeartsNormal
	compare VAR_CONTEST_LOCATION, 2
	call_if_eq LinkContestRoom1_EventScript_GetNumberOfHeartsSuper
	compare VAR_CONTEST_LOCATION, 3
	call_if_eq LinkContestRoom1_EventScript_GetNumberOfHeartsHyper
	compare VAR_CONTEST_LOCATION, 4
	call_if_eq LinkContestRoom1_EventScript_GetNumberOfHeartsMaster
	compare VAR_CONTEST_LOCATION, 5
	call_if_eq LinkContestRoom1_EventScript_GetNumberOfHeartsLink
	setvar VAR_TEMP_1, 9
	setvar VAR_TEMP_2, 9
	setvar VAR_TEMP_3, 9
	setvar VAR_TEMP_4, 9
	setvar VAR_TEMP_5, 9
	setvar VAR_TEMP_6, 9
	setvar VAR_TEMP_7, 9
	setvar VAR_TEMP_8, 9
	compare VAR_TEMP_0, 0
	call_if_gt LinkContestRoom1_EventScript_DisplayHearts
	setvar VAR_TEMP_1, 0
	setvar VAR_TEMP_2, 0
	setvar VAR_TEMP_3, 0
	setvar VAR_TEMP_4, 0
	setvar VAR_TEMP_5, 0
	setvar VAR_TEMP_6, 0
	setvar VAR_TEMP_7, 0
	setvar VAR_TEMP_8, 0
	return

LinkContestRoom1_EventScript_DisplayHearts:: @ 81A54EB
	setvar VAR_RESULT, 8
	special ScriptRandom
	compare VAR_RESULT, 0
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember1
	compare VAR_RESULT, 1
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember2
	compare VAR_RESULT, 2
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember3
	compare VAR_RESULT, 3
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember4
	compare VAR_RESULT, 4
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember5
	compare VAR_RESULT, 5
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember6
	compare VAR_RESULT, 6
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember7
	compare VAR_RESULT, 7
	call_if_eq LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember8
	compare VAR_TEMP_0, 0
	goto_if_gt LinkContestRoom1_EventScript_DisplayHearts
	waitmovement 0
	return

LinkContestRoom1_EventScript_GetNumberOfHeartsNormal:: @ 81A555A
	compare VAR_0x8004, 80
	goto_if_gt LinkContestRoom1_EventScript_Set8Hearts
	compare VAR_0x8004, 70
	goto_if_gt LinkContestRoom1_EventScript_Set7Hearts
	compare VAR_0x8004, 60
	goto_if_gt LinkContestRoom1_EventScript_Set6Hearts
	compare VAR_0x8004, 50
	goto_if_gt LinkContestRoom1_EventScript_Set5Hearts
	compare VAR_0x8004, 40
	goto_if_gt LinkContestRoom1_EventScript_Set4Hearts
	compare VAR_0x8004, 30
	goto_if_gt LinkContestRoom1_EventScript_Set3Hearts
	compare VAR_0x8004, 20
	goto_if_gt LinkContestRoom1_EventScript_Set2Hearts
	compare VAR_0x8004, 10
	goto_if_gt LinkContestRoom1_EventScript_Set1Heart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_GetNumberOfHeartsSuper:: @ 81A55B8
	compare VAR_0x8004, 230
	goto_if_gt LinkContestRoom1_EventScript_Set8Hearts
	compare VAR_0x8004, 210
	goto_if_gt LinkContestRoom1_EventScript_Set7Hearts
	compare VAR_0x8004, 190
	goto_if_gt LinkContestRoom1_EventScript_Set6Hearts
	compare VAR_0x8004, 170
	goto_if_gt LinkContestRoom1_EventScript_Set5Hearts
	compare VAR_0x8004, 150
	goto_if_gt LinkContestRoom1_EventScript_Set4Hearts
	compare VAR_0x8004, 130
	goto_if_gt LinkContestRoom1_EventScript_Set3Hearts
	compare VAR_0x8004, 110
	goto_if_gt LinkContestRoom1_EventScript_Set2Hearts
	compare VAR_0x8004, 90
	goto_if_gt LinkContestRoom1_EventScript_Set1Heart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_GetNumberOfHeartsHyper:: @ 81A5616
	compare VAR_0x8004, 380
	goto_if_gt LinkContestRoom1_EventScript_Set8Hearts
	compare VAR_0x8004, 350
	goto_if_gt LinkContestRoom1_EventScript_Set7Hearts
	compare VAR_0x8004, 320
	goto_if_gt LinkContestRoom1_EventScript_Set6Hearts
	compare VAR_0x8004, 290
	goto_if_gt LinkContestRoom1_EventScript_Set5Hearts
	compare VAR_0x8004, 260
	goto_if_gt LinkContestRoom1_EventScript_Set4Hearts
	compare VAR_0x8004, 230
	goto_if_gt LinkContestRoom1_EventScript_Set3Hearts
	compare VAR_0x8004, 200
	goto_if_gt LinkContestRoom1_EventScript_Set2Hearts
	compare VAR_0x8004, 170
	goto_if_gt LinkContestRoom1_EventScript_Set1Heart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_GetNumberOfHeartsMaster:: @ 81A5674
	compare VAR_0x8004, 600
	goto_if_gt LinkContestRoom1_EventScript_Set8Hearts
	compare VAR_0x8004, 560
	goto_if_gt LinkContestRoom1_EventScript_Set7Hearts
	compare VAR_0x8004, 520
	goto_if_gt LinkContestRoom1_EventScript_Set6Hearts
	compare VAR_0x8004, 480
	goto_if_gt LinkContestRoom1_EventScript_Set5Hearts
	compare VAR_0x8004, 440
	goto_if_gt LinkContestRoom1_EventScript_Set4Hearts
	compare VAR_0x8004, 400
	goto_if_gt LinkContestRoom1_EventScript_Set3Hearts
	compare VAR_0x8004, 360
	goto_if_gt LinkContestRoom1_EventScript_Set2Hearts
	compare VAR_0x8004, 320
	goto_if_gt LinkContestRoom1_EventScript_Set1Heart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_GetNumberOfHeartsLink:: @ 81A56D2
	compare VAR_0x8004, 600
	goto_if_gt LinkContestRoom1_EventScript_Set8Hearts
	compare VAR_0x8004, 550
	goto_if_gt LinkContestRoom1_EventScript_Set7Hearts
	compare VAR_0x8004, 500
	goto_if_gt LinkContestRoom1_EventScript_Set6Hearts
	compare VAR_0x8004, 450
	goto_if_gt LinkContestRoom1_EventScript_Set5Hearts
	compare VAR_0x8004, 400
	goto_if_gt LinkContestRoom1_EventScript_Set4Hearts
	compare VAR_0x8004, 300
	goto_if_gt LinkContestRoom1_EventScript_Set3Hearts
	compare VAR_0x8004, 200
	goto_if_gt LinkContestRoom1_EventScript_Set2Hearts
	compare VAR_0x8004, 100
	goto_if_gt LinkContestRoom1_EventScript_Set1Heart
	setvar VAR_TEMP_0, 0
	return

LinkContestRoom1_EventScript_Set1Heart:: @ 81A5730
	setvar VAR_TEMP_0, 1
	return

LinkContestRoom1_EventScript_Set2Hearts:: @ 81A5736
	setvar VAR_TEMP_0, 2
	return

LinkContestRoom1_EventScript_Set3Hearts:: @ 81A573C
	setvar VAR_TEMP_0, 3
	return

LinkContestRoom1_EventScript_Set4Hearts:: @ 81A5742
	setvar VAR_TEMP_0, 4
	return

LinkContestRoom1_EventScript_Set5Hearts:: @ 81A5748
	setvar VAR_TEMP_0, 5
	return

LinkContestRoom1_EventScript_Set6Hearts:: @ 81A574E
	setvar VAR_TEMP_0, 6
	return

LinkContestRoom1_EventScript_Set7Hearts:: @ 81A5754
	setvar VAR_TEMP_0, 7
	return

LinkContestRoom1_EventScript_Set8Hearts:: @ 81A575A
	setvar VAR_TEMP_0, 8
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember1:: @ 81A5760
	compare VAR_TEMP_1, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember1AlreadyEmoted
	applymovement 6, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_1, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember1AlreadyEmoted:: @ 81A5783
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember2:: @ 81A5784
	compare VAR_TEMP_2, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember2AlreadyEmoted
	applymovement 12, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_2, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember2AlreadyEmoted:: @ 81A57A7
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember3:: @ 81A57A8
	compare VAR_TEMP_3, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember3AlreadyEmoted
	applymovement 7, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_3, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember3AlreadyEmoted:: @ 81A57CB
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember4:: @ 81A57CC
	compare VAR_TEMP_4, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember4AlreadyEmoted
	applymovement 8, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_4, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember4AlreadyEmoted:: @ 81A57EF
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember5:: @ 81A57F0
	compare VAR_TEMP_5, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember5AlreadyEmoted
	applymovement 9, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_5, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember5AlreadyEmoted:: @ 81A5813
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember6:: @ 81A5814
	compare VAR_TEMP_6, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember6AlreadyEmoted
	applymovement 10, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_6, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember6AlreadyEmoted:: @ 81A5837
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember7:: @ 81A5838
	compare VAR_TEMP_7, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember7AlreadyEmoted
	applymovement 11, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_7, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember7AlreadyEmoted:: @ 81A585B
	return

LinkContestRoom1_EventScript_TryDisplayHeartAudienceMember8:: @ 81A585C
	compare VAR_TEMP_8, 1
	goto_if_eq LinkContestRoom1_EventScript_AudienceMember8AlreadyEmoted
	applymovement 15, LinkContestRoom1_Movement_Heart
	playse SE_PIN
	delay 14
	setvar VAR_TEMP_8, 1
	addvar VAR_TEMP_0, -1
	return

LinkContestRoom1_EventScript_AudienceMember8AlreadyEmoted:: @ 81A587F
	return

LinkContestRoom1_EventScript_ContestantReturn:: @ 81A5880
	closemessage
	release
	removeobject 13
	special sub_80C5164
	switch VAR_0x8006
	case 0, LinkContestRoom1_EventScript_Player1WalkBack
	case 1, LinkContestRoom1_EventScript_Player2WalkBack
	case 2, LinkContestRoom1_EventScript_Player3WalkBack
	case 3, LinkContestRoom1_EventScript_Player4WalkBack
	return

LinkContestRoom1_EventScript_Player1WalkBack:: @ 81A58BA
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_Player1WalkBack
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_Player2WalkBack:: @ 81A58C7
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_Player2WalkBack
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_Player3WalkBack:: @ 81A58D4
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_Player3WalkBack
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_Player4WalkBack:: @ 81A58E1
	lockall
	applymovement VAR_0x800B, LinkContestRoom1_Movement_Player4WalkBack
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_DoContestAppeals:: @ 81A58EE
	lockall
	applymovement 1, LinkContestRoom1_Movement_FaceContestants2
	waitmovement 0
	call LinkContestRoom1_EventScript_LetsAppeal
	waitmessage
	applymovement 1, LinkContestRoom1_Movement_WalkStageLeft
	applymovement 2, LinkContestRoom1_Movement_WalkStageRight
	waitmovement 0
	releaseall
	setvar VAR_TEMP_9, 1
	startcontest
	setvar VAR_TEMP_9, 0
	lockall
	applymovement 1, LinkContestRoom1_Movement_WalkStageRight
	applymovement 2, LinkContestRoom1_Movement_WalkStageLeft
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_LetsAppeal:: @ 81A5930
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_LetsAppealLink
	msgbox LinkContestRoom1_Text_VotingCompleteLetsAppeal, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_LetsAppealLink:: @ 81A5944
	messageautoscroll LinkContestRoom1_Text_VotingCompleteLetsAppeal
	return

LinkContestRoom1_EventScript_ContestResults:: @ 81A594A
	call LinkContestRoom1_EventScript_ThatsItForJudging
	call LinkContestRoom1_EventScript_ThankYouForAppeals
	applymovement 1, LinkContestRoom1_Movement_MCFaceJudge
	waitmovement 0
	call LinkContestRoom1_EventScript_JudgeLooksReady
	call LinkContestRoom1_EventScript_WeWillDeclareWinner
	applymovement 1, LinkContestRoom1_Movement_FaceContestants
	waitmovement 0
	closemessage
	releaseall
	setvar VAR_TEMP_9, 1
	showcontestresults
	setvar VAR_TEMP_9, 0
	playbgm MUS_CONTEST_WINNER, FALSE
	return

LinkContestRoom1_EventScript_ThatsItForJudging:: @ 81A5984
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_ThatsItForJudgingLink
	msgbox LinkContestRoom1_Text_ThatsItForJudging, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_ThatsItForJudgingLink:: @ 81A5998
	messageautoscroll LinkContestRoom1_Text_ThatsItForJudging
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_ThankYouForAppeals:: @ 81A59A2
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_ThankYouForAppealsLink
	msgbox LinkContestRoom1_Text_ThankYouForAppeals, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_ThankYouForAppealsLink:: @ 81A59B6
	messageautoscroll LinkContestRoom1_Text_ThankYouForAppeals
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_JudgeLooksReady:: @ 81A59C0
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_JudgeLooksReadyLink
	msgbox LinkContestRoom1_Text_JudgeLooksReady, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_JudgeLooksReadyLink:: @ 81A59D4
	messageautoscroll LinkContestRoom1_Text_JudgeLooksReady
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_WeWillDeclareWinner:: @ 81A59DE
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_WeWillDeclareWinnerLink
	msgbox LinkContestRoom1_Text_WeWillNowDeclareWinner, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_WeWillDeclareWinnerLink:: @ 81A59F2
	messageautoscroll LinkContestRoom1_Text_WeWillNowDeclareWinner
	waitmessage
	delay 30
	return

LinkContestRoom1_EventScript_GetWinnerObjEventId:: @ 81A59FC
	special ScrSpecial_GetContestWinnerIdx
	switch VAR_0x8005
	case 0, LinkContestRoom1_EventScript_GetPlayer1ObjEventId
	case 1, LinkContestRoom1_EventScript_GetPlayer2ObjEventId
	case 2, LinkContestRoom1_EventScript_GetPlayer3ObjEventId
	case 3, LinkContestRoom1_EventScript_GetPlayer4ObjEventId
	return

LinkContestRoom1_EventScript_GetPlayer1ObjEventId:: @ 81A5A31
	setvar VAR_TEMP_3, 3
	return

LinkContestRoom1_EventScript_GetPlayer2ObjEventId:: @ 81A5A37
	setvar VAR_TEMP_3, 4
	return

LinkContestRoom1_EventScript_GetPlayer3ObjEventId:: @ 81A5A3D
	setvar VAR_TEMP_3, 5
	return

LinkContestRoom1_EventScript_GetPlayer4ObjEventId:: @ 81A5A43
	setvar VAR_TEMP_3, 14
	return

LinkContestRoom1_EventScript_CongratulateWinner:: @ 81A5A49
	special ScrSpecial_GetContestWinnerTrainerName
	special ScrSpecial_GetContestWinnerNick
	addvar VAR_0x8005, 1
	buffernumberstring 1, VAR_0x8005
	addvar VAR_0x8005, -1
	call LinkContestRoom1_EventScript_CongratsWinner
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_WinningPlayerWalkUp
	waitmovement 0
	playse SE_M_ENCORE2
	setvar VAR_TEMP_1, 0
	return

LinkContestRoom1_EventScript_CongratsWinner:: @ 81A5A75
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_CongratsWinnerLink
	msgbox LinkContestRoom1_Text_CongratsTrainerXandMon, MSGBOX_DEFAULT
	return

LinkContestRoom1_EventScript_CongratsWinnerLink:: @ 81A5A89
	messageautoscroll LinkContestRoom1_Text_CongratsTrainerXandMon
	waitmessage
	return

LinkContestRoom1_EventScript_AudienceLookAround:: @ 81A5A90
	addvar VAR_TEMP_1, 1
	lockall
	compare VAR_CONTEST_LOCATION, 1
	call_if_gt LinkContestRoom1_EventScript_VObjectAudienceLookAround
	applymovement 9, LinkContestRoom1_Movement_AudienceMemberLookRight
	applymovement 12, LinkContestRoom1_Movement_AudienceMemberLookDown
	applymovement 7, LinkContestRoom1_Movement_AudienceMemberLookRight
	delay 30
	applymovement 10, LinkContestRoom1_Movement_AudienceMemberLookLeft
	applymovement 11, LinkContestRoom1_Movement_AudienceMemberLookDown
	applymovement 6, LinkContestRoom1_Movement_AudienceMemberLookUp
	applymovement 8, LinkContestRoom1_Movement_AudienceMemberLookLeft
	compare VAR_TEMP_1, 4
	goto_if_ne LinkContestRoom1_EventScript_AudienceLookAround
	delay 30
	return

LinkContestRoom1_EventScript_VObjectAudienceLookAround:: @ 81A5AE4
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

LinkContestRoom1_EventScript_GiveWinnerPrize:: @ 81A5BAB
	compare VAR_CONTEST_LOCATION, 5
	goto_if_eq LinkContestRoom1_EventScript_EndLinkContest
	call LinkContestRoom1_EventScript_CheckShouldSkipPrize
	goto_if_set FLAG_TEMP_2, LinkContestRoom1_EventScript_SkipPrize
	msgbox LinkContestRoom1_Text_AcceptYourPrize, MSGBOX_SIGN
	waitmessage
	call LinkContestRoom1_EventScript_WinnerApproachForPrize
	call LinkContestRoom1_EventScript_GivePrizeIfWinner
	playse SE_M_ENCORE2
	setvar VAR_TEMP_1, 0
	call LinkContestRoom1_EventScript_AudienceLookAround
	delay 30
	special ShouldReadyContestArtist
	compare VAR_0x8004, 1
	goto_if_eq LinkContestRoom1_EventScript_SetReadyForContestArtist
	return

LinkContestRoom1_EventScript_SkipPrize:: @ 81A5BF6
	incrementgamestat GAME_STAT_WON_CONTEST
	msgbox LinkContestRoom1_Text_CongratsPleaseCompeteAgain, MSGBOX_SIGN
	delay 90
	special ShouldReadyContestArtist
	compare VAR_0x8004, 1
	goto_if_eq LinkContestRoom1_EventScript_SetReadyForContestArtist
	return

LinkContestRoom1_EventScript_CheckShouldSkipPrize:: @ 81A5C12
	specialvar VAR_RESULT, ScrSpecial_CanMonParticipateInSelectedLinkContest
	compare VAR_RESULT, 1
	goto_if_eq LinkContestRoom1_EventScript_CheckPlayerWon
	return

LinkContestRoom1_EventScript_CheckPlayerWon:: @ 81A5C23
	special ScrSpecial_GetContestWinnerIdx
	compare VAR_0x8005, 3
	goto_if_eq LinkContestRoom1_EventScript_CheckRankIsMaster
	return

LinkContestRoom1_EventScript_CheckRankIsMaster:: @ 81A5C32
	compare VAR_CONTEST_RANK, 3
	goto_if_eq LinkContestRoom1_EventScript_DontSkipPrize
	setflag FLAG_TEMP_2
	return

LinkContestRoom1_EventScript_DontSkipPrize:: @ 81A5C41
	return

LinkContestRoom1_EventScript_SetSketchFlag:: @ 81A5C42
	setflag FLAG_CONTEST_SKETCH_CREATED
	return

LinkContestRoom1_EventScript_SetReadyForContestArtist:: @ 81A5C46
	setvar VAR_LILYCOVE_CONTEST_LOBBY_STATE, 1
	return

LinkContestRoom1_EventScript_EndLinkContest:: @ 81A5C4C
	delay 60
	call LinkContestRoom1_EventScript_CheckLinkContestPlayerWon
	special ShouldReadyContestArtist
	compare VAR_0x8004, 1
	goto_if_eq LinkContestRoom1_EventScript_SetReadyForLinkContestArtist
	closemessage
	return

LinkContestRoom1_EventScript_SetReadyForLinkContestArtist:: @ 81A5C64
	setvar VAR_LILYCOVE_CONTEST_LOBBY_STATE, 2
	return

LinkContestRoom1_EventScript_CheckLinkContestPlayerWon:: @ 81A5C6A
	special ScrSpecial_GetContestPlayerMonIdx
	special ScrSpecial_GetContestWinnerIdx
	compare VAR_0x8004, VAR_0x8005
	goto_if_eq LinkContestRoom1_EventScript_RecordLinkContestWin
	return

LinkContestRoom1_EventScript_RecordLinkContestWin:: @ 81A5C7C
	incrementgamestat GAME_STAT_WON_LINK_CONTEST
	return

LinkContestRoom1_EventScript_WinnerApproachForPrize:: @ 81A5C7F
	switch VAR_0x8005
	case 0, LinkContestRoom1_EventScript_Player1ApproachForPrize
	case 1, LinkContestRoom1_EventScript_Player2ApproachForPrize
	case 2, LinkContestRoom1_EventScript_Player3ApproachForPrize
	case 3, LinkContestRoom1_EventScript_Player4ApproachForPrize
	return

LinkContestRoom1_EventScript_Player1ApproachForPrize:: @ 81A5CB1
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_Player1ApproachForPrize
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_Player2ApproachForPrize:: @ 81A5CBE
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_Player2ApproachForPrize
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_Player3ApproachForPrize:: @ 81A5CCB
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_Player3ApproachForPrize
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_Player4ApproachForPrize:: @ 81A5CD8
	lockall
	applymovement VAR_TEMP_3, LinkContestRoom1_Movement_Player4ApproachForPrize
	waitmovement 0
	releaseall
	return

LinkContestRoom1_EventScript_GivePrizeIfWinner:: @ 81A5CE5
	special ScrSpecial_GetContestWinnerIdx
	compare VAR_0x8005, 3
	goto_if_eq LinkContestRoom1_EventScript_GiveContestPrizes
	msgbox LinkContestRoom1_Text_CongratsPleaseCompeteAgain, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_GiveContestPrizes:: @ 81A5CFC
	compare VAR_CONTEST_LOCATION, 2
	call_if_eq LinkContestRoom1_EventScript_SetSketchFlag
	incrementgamestat GAME_STAT_WON_CONTEST
	specialvar VAR_RESULT, ScrSpecial_CanMonParticipateInSelectedLinkContest
	compare VAR_RESULT, 0
	goto_if_eq LinkContestRoom1_EventScript_ReceiveContestRibbon
	compare VAR_CONTEST_RANK, 3
	goto_if_eq LinkContestRoom1_EventScript_GiveLuxuryBall
	msgbox LinkContestRoom1_Text_CongratsPleaseCompeteAgain, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_NoRoomForLuxuryBall:: @ 81A5D2D
	msgbox LinkContestRoom1_Text_PickUpPrizeAtCounterLater, MSGBOX_SIGN
	setvar VAR_CONTEST_PRIZE_PICKUP, 4
	return

LinkContestRoom1_EventScript_GiveLuxuryBall:: @ 81A5D3B
	giveitem ITEM_LUXURY_BALL
	compare VAR_RESULT, 0
	goto_if_eq LinkContestRoom1_EventScript_NoRoomForLuxuryBall
	msgbox LinkContestRoom1_Text_CongratsPleaseCompeteAgain, MSGBOX_SIGN
	return

LinkContestRoom1_EventScript_ReceiveContestRibbon:: @ 81A5D5B
	special ScrSpecial_GiveContestRibbon
	incrementgamestat GAME_STAT_RECEIVED_RIBBONS
	setflag FLAG_SYS_RIBBON_GET
	lockall
	msgbox LinkContestRoom1_Text_ConferRibbonAsPrize, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	msgbox LinkContestRoom1_Text_ReceivedRibbon, MSGBOX_DEFAULT
	waitfanfare
	special ScrSpecial_GetContestWinnerNick
	msgbox LinkContestRoom1_Text_PutRibbonOnMon, MSGBOX_DEFAULT
	releaseall
	return

LinkContestRoom1_Movement_MCWalkDown:: @ 81A5D85
	walk_down
	step_end

LinkContestRoom1_Movement_MCBackUp:: @ 81A5D87
	lock_facing_direction
	walk_up
	unlock_facing_direction
	step_end

LinkContestRoom1_Movement_MCFaceJudge:: @ 81A5D8B
	walk_in_place_fastest_right
	step_end

LinkContestRoom1_Movement_Heart:: @ 81A5D8D
	emote_heart
	step_end

LinkContestRoom1_Movement_FaceContestants:: @ 81A5D8F
	walk_in_place_fastest_down
	step_end

LinkContestRoom1_Movement_WalkStageLeft:: @ 81A5D91
	walk_left
	walk_left
	walk_in_place_fastest_down
	step_end

LinkContestRoom1_Movement_WalkStageRight:: @ 81A5D95
	walk_right
	walk_right
	walk_in_place_fastest_down
	step_end

LinkContestRoom1_Movement_WinningPlayerWalkUp:: @ 81A5D99
	walk_up
	walk_in_place_fastest_down
	step_end

LinkContestRoom1_Movement_ContestantDelay32:: @ 81A5D9C
	delay_16
	delay_16
	step_end

LinkContestRoom1_Movement_MCFaceJudge2:: @ 81A5D9F
	walk_in_place_fastest_right
	step_end

LinkContestRoom1_Movement_JudgeFaceMC:: @ 81A5DA1
	walk_in_place_fastest_left
	step_end

LinkContestRoom1_Movement_FaceContestants2:: @ 81A5DA3
	walk_in_place_fastest_down
	step_end

LinkContestRoom1_Movement_Player3ApproachForPrize:: @ 81A5DA5
	walk_left
	walk_left
	walk_up
	step_end

LinkContestRoom1_Movement_Player4ApproachForPrize:: @ 81A5DA9
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	step_end

LinkContestRoom1_Movement_AudienceMemberLookLeft:: @ 81A5DAF
	face_left
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_AudienceMemberLookUp:: @ 81A5DB3
	face_up
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_AudienceMemberLookRight:: @ 81A5DB7
	face_right
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_AudienceMemberLookDown:: @ 81A5DBB
	face_down
	delay_16
	face_original_direction
	step_end

LinkContestRoom1_Movement_Player4FaceUp:: @ 81A5DBF
	face_up
	step_end

LinkContestRoom1_Movement_MCLookAtJudge:: @ 81A5DC1
	face_up
	delay_16
	walk_in_place_fastest_right
	step_end

LinkContestRoom1_Movement_JudgeLookAtMC:: @ 81A5DC5
	walk_in_place_fastest_left
	delay_16
	delay_16
	walk_in_place_fastest_down
	step_end

LinkContestRoom1_Movement_MCWalkInPlaceDown:: @ 81A5DCA
	delay_16
	delay_16
	walk_in_place_fastest_down
	step_end

LinkContestRoom1_Movement_Player1WalkToCenter:: @ 81A5DCE
	walk_up
	walk_right
	walk_right
	walk_right
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player1WalkBack:: @ 81A5DD4
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_down
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player2WalkToCenter:: @ 81A5DDA
	walk_up
	walk_right
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player2WalkBack:: @ 81A5DDE
	walk_fast_left
	walk_fast_down
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player3WalkToCenter:: @ 81A5DE2
	walk_up
	walk_left
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player3WalkBack:: @ 81A5DE6
	walk_fast_right
	walk_fast_down
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player4WalkToCenter:: @ 81A5DEA
	walk_up
	walk_left
	walk_left
	walk_left
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player4WalkBack:: @ 81A5DF0
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_in_place_fastest_up
	step_end

LinkContestRoom1_Movement_Player1ApproachForPrize:: @ 81A5DF6
	walk_right
	walk_right
	walk_up
	step_end

LinkContestRoom1_Movement_Player2ApproachForPrize:: @ 81A5DFA
	walk_up
	step_end
