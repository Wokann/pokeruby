GabbyAndTy_EventScript_UpdateLocation:: @ 81AE313
	cleartrainerflag TRAINER_GABBY_AND_TY_6
	specialvar VAR_RESULT, GabbyAndTyGetBattleNum
	switch VAR_RESULT
	case 0, GabbyAndTy_EventScript_MoveForBattle1
	case 1, GabbyAndTy_EventScript_MoveForBattle2
	case 2, GabbyAndTy_EventScript_MoveForBattle3
	case 3, GabbyAndTy_EventScript_MoveForBattle4
	case 4, GabbyAndTy_EventScript_MoveForBattle5
	case 5, GabbyAndTy_EventScript_MoveForBattle6
	case 6, GabbyAndTy_EventScript_MoveForBattle7
	case 7, GabbyAndTy_EventScript_MoveForBattle8
	case 8, GabbyAndTy_EventScript_MoveForBattle9
	end

GabbyAndTy_EventScript_MoveForBattle1:: @ 81AE384
	call GabbyAndTy_EventScript_ShowAtRoute111_1
	return

GabbyAndTy_EventScript_MoveForBattle2:: @ 81AE38A
	call GabbyAndTy_EventScript_ShowAtRoute118_1
	call GabbyAndTy_EventScript_HideAtRoute111_1
	return

GabbyAndTy_EventScript_MoveForBattle3:: @ 81AE395
	call GabbyAndTy_EventScript_ShowAtRoute120_1
	call GabbyAndTy_EventScript_HideAtRoute118_1
	return

GabbyAndTy_EventScript_MoveForBattle4:: @ 81AE3A0
	call GabbyAndTy_EventScript_ShowAtRoute111_2
	call GabbyAndTy_EventScript_HideAtRoute120_1
	return

GabbyAndTy_EventScript_MoveForBattle5:: @ 81AE3AB
	call GabbyAndTy_EventScript_ShowAtRoute118_2
	call GabbyAndTy_EventScript_HideAtRoute111_2
	return

GabbyAndTy_EventScript_MoveForBattle6:: @ 81AE3B6
	call GabbyAndTy_EventScript_ShowAtRoute120_2
	call GabbyAndTy_EventScript_HideAtRoute118_2
	return

GabbyAndTy_EventScript_MoveForBattle7:: @ 81AE3C1
	call GabbyAndTy_EventScript_ShowAtRoute111_3
	call GabbyAndTy_EventScript_HideAtRoute120_2
	return

GabbyAndTy_EventScript_MoveForBattle8:: @ 81AE3CC
	call GabbyAndTy_EventScript_ShowAtRoute118_3
	call GabbyAndTy_EventScript_HideAtRoute111_3
	return

GabbyAndTy_EventScript_MoveForBattle9:: @ 81AE3D7
	call GabbyAndTy_EventScript_ShowAtRoute120_2
	call GabbyAndTy_EventScript_HideAtRoute118_3
	return

GabbyAndTy_EventScript_HideAtRoute111_1:: @ 81AE3E2
	setflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_1
	return

GabbyAndTy_EventScript_ShowAtRoute111_1:: @ 81AE3E6
	clearflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_1
	return

GabbyAndTy_EventScript_HideAtRoute118_1:: @ 81AE3EA
	setflag FLAG_HIDE_GABBY_AND_TY_ROUTE118_1
	return

GabbyAndTy_EventScript_ShowAtRoute118_1:: @ 81AE3EE
	clearflag FLAG_HIDE_GABBY_AND_TY_ROUTE118_1
	return

GabbyAndTy_EventScript_HideAtRoute120_1:: @ 81AE3F2
	setflag FLAG_HIDE_GABBY_AND_TY_ROUTE120_1
	return

GabbyAndTy_EventScript_ShowAtRoute120_1:: @ 81AE3F6
	clearflag FLAG_HIDE_GABBY_AND_TY_ROUTE120_1
	return

GabbyAndTy_EventScript_HideAtRoute111_2:: @ 81AE3FA
	setflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_2
	return

GabbyAndTy_EventScript_ShowAtRoute111_2:: @ 81AE3FE
	clearflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_2
	return

GabbyAndTy_EventScript_HideAtRoute118_2:: @ 81AE402
	setflag FLAG_HIDE_GABBY_AND_TY_ROUTE118_2
	return

GabbyAndTy_EventScript_ShowAtRoute118_2:: @ 81AE406
	clearflag FLAG_HIDE_GABBY_AND_TY_ROUTE118_2
	return

GabbyAndTy_EventScript_HideAtRoute120_2:: @ 81AE40A
	setflag FLAG_HIDE_GABBY_AND_TY_ROUTE120_2
	return

GabbyAndTy_EventScript_ShowAtRoute120_2:: @ 81AE40E
	clearflag FLAG_HIDE_GABBY_AND_TY_ROUTE120_2
	return

GabbyAndTy_EventScript_HideAtRoute111_3:: @ 81AE412
	setflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_3
	return

GabbyAndTy_EventScript_ShowAtRoute111_3:: @ 81AE416
	clearflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_3
	return

GabbyAndTy_EventScript_HideAtRoute118_3:: @ 81AE41A
	setflag FLAG_HIDE_GABBY_AND_TY_ROUTE118_3
	return

GabbyAndTy_EventScript_ShowAtRoute118_3:: @ 81AE41E
	clearflag FLAG_HIDE_GABBY_AND_TY_ROUTE118_3
	return

GabbyAndTy_EventScript_GabbyBattle1:: @ 81AE422
	trainerbattle_double TRAINER_GABBY_AND_TY_1, GabbyAndTy_Text_GabbyPreFirstBattle, GabbyAndTy_Text_GabbyDefeatFirstTime, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_FirstInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle1:: @ 81AE442
	trainerbattle_double TRAINER_GABBY_AND_TY_1, GabbyAndTy_Text_TyPreFirstBattle, GabbyAndTy_Text_TyDefeatFirstTime, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_FirstInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle2:: @ 81AE462
	trainerbattle_double TRAINER_GABBY_AND_TY_2, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle2:: @ 81AE482
	trainerbattle_double TRAINER_GABBY_AND_TY_2, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle3:: @ 81AE4A2
	trainerbattle_double TRAINER_GABBY_AND_TY_3, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle3:: @ 81AE4C2
	trainerbattle_double TRAINER_GABBY_AND_TY_3, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle4:: @ 81AE4E2
	trainerbattle_double TRAINER_GABBY_AND_TY_4, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle4:: @ 81AE502
	trainerbattle_double TRAINER_GABBY_AND_TY_4, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle5:: @ 81AE522
	trainerbattle_double TRAINER_GABBY_AND_TY_5, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle5:: @ 81AE542
	trainerbattle_double TRAINER_GABBY_AND_TY_5, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle6:: @ 81AE562
	trainerbattle_double TRAINER_GABBY_AND_TY_6, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle6:: @ 81AE582
	trainerbattle_double TRAINER_GABBY_AND_TY_6, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_FirstInterview:: @ 81AE5A2
	special GabbyAndTyBeforeInterview
	special GetGabbyAndTyLocalIds
	compare VAR_FACING, 2
	call_if_eq GabbyAndTy_EventScript_FacePlayerNorth
	compare VAR_FACING, 1
	call_if_eq GabbyAndTy_EventScript_FacePlayerSouth
	compare VAR_FACING, 4
	call_if_eq GabbyAndTy_EventScript_FacePlayerEast
	goto_if_set FLAG_TEMP_1, GabbyAndTy_EventScript_KeepingAnEyeOutForYou
	msgbox GabbyAndTy_Text_WhoAreYouInterview, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_FacePlayerNorth:: @ 81AE5E0
	applymovement VAR_0x8004, GabbyAndTy_Movement_WalkInPlaceDown
	waitmovement 0
	return

GabbyAndTy_EventScript_FacePlayerSouth:: @ 81AE5EB
	applymovement VAR_0x8004, GabbyAndTy_Movement_WalkInPlaceUp
	applymovement VAR_0x8005, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	return

GabbyAndTy_EventScript_FacePlayerEast:: @ 81AE5FD
	applymovement VAR_0x8004, GabbyAndTy_Movement_WalkInPlaceLeft
	applymovement VAR_0x8005, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	return

GabbyAndTy_EventScript_RequestInterview:: @ 81AE60F
	special GabbyAndTyBeforeInterview
	special GetGabbyAndTyLocalIds
	compare VAR_FACING, 2
	call_if_eq GabbyAndTy_EventScript_FacePlayerNorth
	compare VAR_FACING, 1
	call_if_eq GabbyAndTy_EventScript_FacePlayerSouth
	compare VAR_FACING, 4
	call_if_eq GabbyAndTy_EventScript_FacePlayerEast
	goto_if_set FLAG_TEMP_1, GabbyAndTy_EventScript_KeepingAnEyeOutForYou
	specialvar VAR_RESULT, GabbyAndTyGetLastQuote
	compare VAR_RESULT, 0
	goto_if_eq GabbyAndTy_EventScript_DidntInterviewLastTime
	msgbox GabbyAndTy_Text_QuoteFromLastInterview, MSGBOX_DEFAULT
	specialvar VAR_RESULT, GabbyAndTyGetLastBattleTrivia
	switch VAR_RESULT
	case 0, GabbyAndTy_EventScript_RequestInterviewNoTrivia
	case 1, GabbyAndTy_EventScript_RequestInterviewShortBattle
	case 2, GabbyAndTy_EventScript_RequestInterviewThrewBall
	case 3, GabbyAndTy_EventScript_RequestInterviewUsedItems
	case 4, GabbyAndTy_EventScript_RequestInterviewLostAMon
	end

GabbyAndTy_Movement_WalkInPlaceDown:: @ 81AE699
	walk_in_place_down
	step_end

GabbyAndTy_Movement_WalkInPlaceUp:: @ 81AE69B
	walk_in_place_up
	step_end

GabbyAndTy_Movement_WalkInPlaceLeft:: @ 81AE69D
	walk_in_place_left
	step_end

GabbyAndTy_EventScript_DidntInterviewLastTime:: @ 81AE69F
	msgbox GabbyAndTy_Text_GiveUsAnInterviewThisTime, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewNoTrivia:: @ 81AE6AD
	msgbox GabbyAndTy_Text_InterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewShortBattle:: @ 81AE6BB
	msgbox GabbyAndTy_Text_YouStompedUsInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewThrewBall:: @ 81AE6C9
	msgbox GabbyAndTy_Text_YouThrewABallAtUsInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewUsedItems:: @ 81AE6D7
	msgbox GabbyAndTy_Text_CleverItemSkillsInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewLostAMon:: @ 81AE6E5
	msgbox GabbyAndTy_Text_WeLookedRespectableInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_Interview:: @ 81AE6F3
	compare VAR_RESULT, NO
	goto_if_eq GabbyAndTy_EventScript_DontGiveUpKeepingEyeOut
	msgbox GabbyAndTy_Text_DescribeYourFeelings, MSGBOX_DEFAULT
	setvar VAR_0x8004, 10
	call Common_EventScript_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 0
	goto_if_eq GabbyAndTy_EventScript_DontGiveUpKeepingEyeOut
	msgbox GabbyAndTy_Text_PerfectWellBeSeeingYou, MSGBOX_DEFAULT
	special GabbyAndTyAfterInterview
	setflag FLAG_TEMP_1
	release
	end

GabbyAndTy_EventScript_DontGiveUpKeepingEyeOut:: @ 81AE72D
	msgbox GabbyAndTy_Text_DontGiveUpKeepingEyeOut, MSGBOX_DEFAULT
	setflag FLAG_TEMP_1
	release
	end

GabbyAndTy_EventScript_KeepingAnEyeOutForYou:: @ 81AE73A
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end
