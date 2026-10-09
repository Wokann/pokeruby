SlateportCity_ContestHall_MapScripts:: @ 815554B
	.byte 0

SlateportCity_ContestHall_EventScript_15554C:: @ 815554C
	msgbox SlateportCity_ContestHall_Text_AdviceForContests, MSGBOX_NPC
	end

SlateportCity_ContestHall_EventScript_155555:: @ 8155555
	msgbox SlateportCity_ContestHall_Text_MyPapaIsContestJudge, MSGBOX_NPC
	end

SlateportCity_ContestHall_EventScript_15555E:: @ 815555E
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM_TORMENT, SlateportCity_ContestHall_EventScript_155595
	msgbox SlateportCity_ContestHall_Text_ImLikeMajorlyCheesed, MSGBOX_DEFAULT
	giveitem ITEM_TM_TORMENT
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM_TORMENT
	msgbox SlateportCity_ContestHall_Text_ExplainTorment, MSGBOX_DEFAULT
	release
	end

SlateportCity_ContestHall_EventScript_155595:: @ 8155595
	msgbox SlateportCity_ContestHall_Text_ExplainTorment, MSGBOX_DEFAULT
	release
	end

SlateportCity_ContestHall_EventScript_15559F:: @ 815559F
	msgbox SlateportCity_ContestHall_Text_MCStepUpTakePartInContest, MSGBOX_NPC
	lockall
	applymovement 3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_1555B4:: @ 81555B4
	msgbox SlateportCity_ContestHall_Text_JudgeWouldntDoToMissContest, MSGBOX_NPC
	lockall
	applymovement 4, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_1555C9:: @ 81555C9
	msgbox SlateportCity_ContestHall_Text_ItsAppealTime, MSGBOX_NPC
	lockall
	applymovement 5, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_1555DE:: @ 81555DE
	msgbox SlateportCity_ContestHall_Text_DidntPayAttentionToAppeal, MSGBOX_NPC
	lockall
	applymovement 6, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_1555F3:: @ 81555F3
	msgbox SlateportCity_ContestHall_Text_RewardWithSageAdvice, MSGBOX_NPC
	lockall
	applymovement 7, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_155608:: @ 8155608
	msgbox SlateportCity_ContestHall_Text_MoreFreakedOutThanMon, MSGBOX_NPC
	lockall
	applymovement 8, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_15561D:: @ 815561D
	msgbox SlateportCity_ContestHall_Text_MyMonBetterThanThatLot, MSGBOX_NPC
	lockall
	applymovement 9, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_155632:: @ 8155632
	msgbox SlateportCity_ContestHall_Text_GetUrgeToMoveWithMon, MSGBOX_NPC
	lockall
	applymovement 10, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_155647:: @ 8155647
	msgbox SlateportCity_ContestHall_Text_MonLooksOnTopOfGame, MSGBOX_NPC
	lockall
	applymovement 12, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_15565C:: @ 815565C
	msgbox SlateportCity_ContestHall_Text_BattleAndContestAlike, MSGBOX_NPC
	lockall
	applymovement 11, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

SlateportCity_ContestHall_EventScript_155671:: @ 8155671
	msgbox SlateportCity_ContestHall_Text_HyperRankStage, MSGBOX_SIGN
	end
