Route110_TrickHousePuzzle5_MapScripts:: @ 81627B0
	map_script MAP_SCRIPT_ON_TRANSITION, Route110_TrickHousePuzzle5_OnTransition
	.byte 0

Route110_TrickHousePuzzle5_OnTransition:: @ 81627B6
	setvar VAR_TEMP_1, 0
	setvar VAR_TEMP_2, 0
	setvar VAR_TEMP_3, 0
	setvar VAR_TEMP_4, 0
	setvar VAR_TEMP_5, 0
	setvar VAR_TEMP_8, 0
	end

Route110_TrickHousePuzzle5_EventScript_Scroll:: @ 81627D5
	lockall
	compare VAR_TRICK_HOUSE_PUZZLE_5_STATE, 0
	goto_if_eq Route110_TrickHousePuzzle5_EventScript_ReadScroll
	goto Route110_TrickHousePuzzle5_EventScript_ReadScrollAgain
	end

Route110_TrickHousePuzzle5_EventScript_ReadScroll:: @ 81627E7
	setvar VAR_TRICK_HOUSE_PUZZLE_5_STATE, 1
	goto Route110_TrickHousePuzzle5_EventScript_FoundScroll
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1:: @ 81627F2
	lockall
	applymovement 1, Common_Movement_FacePlayer
	waitmovement 0
	compare VAR_TEMP_1, 1
	goto_if_eq Route110_TrickHousePuzzle5_EventScript_CorrectGoThrough
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll1Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll2:: @ 8162813
	lockall
	applymovement 2, Common_Movement_FacePlayer
	waitmovement 0
	compare VAR_TEMP_2, 1
	goto_if_eq Route110_TrickHousePuzzle5_EventScript_CorrectGoThrough
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll2Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3:: @ 8162834
	lockall
	applymovement 3, Common_Movement_FacePlayer
	waitmovement 0
	compare VAR_TEMP_3, 1
	goto_if_eq Route110_TrickHousePuzzle5_EventScript_CorrectGoThrough
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll3Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll4:: @ 8162855
	lockall
	applymovement 4, Common_Movement_FacePlayer
	waitmovement 0
	compare VAR_TEMP_4, 1
	goto_if_eq Route110_TrickHousePuzzle5_EventScript_CorrectGoThrough
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll4Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5:: @ 8162876
	lockall
	applymovement 5, Common_Movement_FacePlayer
	waitmovement 0
	compare VAR_TEMP_5, 1
	goto_if_eq Route110_TrickHousePuzzle5_EventScript_CorrectGoThrough
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll5Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Trigger0:: @ 8162897
	lockall
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll1Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Trigger1:: @ 81628A3
	lockall
	setvar VAR_TEMP_9, 1
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll1Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Trigger2:: @ 81628AF
	lockall
	setvar VAR_TEMP_9, 2
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll1Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Trigger3:: @ 81628BB
	lockall
	setvar VAR_TEMP_9, 3
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll1Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll2Trigger0:: @ 81628C7
	lockall
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll2Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll2Trigger1:: @ 81628D3
	lockall
	setvar VAR_TEMP_9, 1
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll2Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3Trigger0:: @ 81628DF
	lockall
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll3Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3Trigger1:: @ 81628EB
	lockall
	setvar VAR_TEMP_9, 1
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll3Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3Trigger2:: @ 81628F7
	lockall
	setvar VAR_TEMP_9, 2
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll3Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll4Trigger0:: @ 8162903
	lockall
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll4Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll4Trigger1:: @ 816290F
	lockall
	setvar VAR_TEMP_9, 1
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll4Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5Trigger0:: @ 816291B
	lockall
	setvar VAR_TEMP_9, 0
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll5Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5Trigger1:: @ 8162927
	lockall
	setvar VAR_TEMP_9, 1
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll5Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5Trigger2:: @ 8162933
	lockall
	setvar VAR_TEMP_9, 2
	goto Route110_TrickHousePuzzle5_EventScript_Mechadoll5Activate
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Activate:: @ 816293F
	setvar VAR_TEMP_1, 1
	setvar VAR_TEMP_8, 1
	playse SE_PIN
	applymovement 1, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 1, Common_Movement_Delay48
	waitmovement 0
	compare VAR_TEMP_9, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft1
	compare VAR_TEMP_9, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft2
	compare VAR_TEMP_9, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft3
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll1Intro, MSGBOX_DEFAULT
	random 3
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz1
	case 1, Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz2
	case 2, Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz3
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll2Activate:: @ 81629B3
	setvar VAR_TEMP_2, 1
	setvar VAR_TEMP_8, 2
	playse SE_PIN
	applymovement 2, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 2, Common_Movement_Delay48
	waitmovement 0
	compare VAR_TEMP_9, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight1
	compare VAR_TEMP_9, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight2
	compare VAR_TEMP_9, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight3
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll2Intro, MSGBOX_DEFAULT
	random 3
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz1
	case 1, Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz2
	case 2, Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz3
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3Activate:: @ 8162A27
	setvar VAR_TEMP_3, 1
	setvar VAR_TEMP_8, 3
	playse SE_PIN
	applymovement 3, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 3, Common_Movement_Delay48
	waitmovement 0
	compare VAR_TEMP_9, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft1
	compare VAR_TEMP_9, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft2
	compare VAR_TEMP_9, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft3
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll3Intro, MSGBOX_DEFAULT
	random 3
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz1
	case 1, Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz2
	case 2, Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz3
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll4Activate:: @ 8162A9B
	setvar VAR_TEMP_4, 1
	setvar VAR_TEMP_8, 4
	playse SE_PIN
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_Delay48
	waitmovement 0
	compare VAR_TEMP_9, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft1
	compare VAR_TEMP_9, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft2
	compare VAR_TEMP_9, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft3
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll4Intro, MSGBOX_DEFAULT
	random 3
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz1
	case 1, Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz2
	case 2, Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz3
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5Activate:: @ 8162B0F
	setvar VAR_TEMP_5, 1
	setvar VAR_TEMP_8, 5
	playse SE_PIN
	applymovement 5, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 5, Common_Movement_Delay48
	waitmovement 0
	compare VAR_TEMP_9, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight1
	compare VAR_TEMP_9, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight2
	compare VAR_TEMP_9, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight3
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll5Intro, MSGBOX_DEFAULT
	random 3
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz1
	case 1, Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz2
	case 2, Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz3
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz1:: @ 8162B83
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz1, MSGBOX_DEFAULT
	multichoice 0, 0, 25, 1
	switch VAR_RESULT
	case 1, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz2:: @ 8162BA6
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz2, MSGBOX_DEFAULT
	multichoice 0, 0, 26, 1
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll1Quiz3:: @ 8162BC9
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll1Quiz3, MSGBOX_DEFAULT
	multichoice 0, 0, 27, 1
	switch VAR_RESULT
	case 1, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz1:: @ 8162BEC
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz1, MSGBOX_DEFAULT
	multichoice 0, 0, 28, 1
	switch VAR_RESULT
	case 2, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz2:: @ 8162C0F
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz2, MSGBOX_DEFAULT
	multichoice 0, 0, 29, 1
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll2Quiz3:: @ 8162C32
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll2Quiz3, MSGBOX_DEFAULT
	multichoice 0, 0, 30, 1
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz1:: @ 8162C55
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz1, MSGBOX_DEFAULT
	multichoice 0, 0, 31, 1
	switch VAR_RESULT
	case 1, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz2:: @ 8162C78
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz2, MSGBOX_DEFAULT
	multichoice 0, 0, 32, 1
	switch VAR_RESULT
	case 2, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll3Quiz3:: @ 8162C9B
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll3Quiz3, MSGBOX_DEFAULT
	multichoice 0, 0, 33, 1
	switch VAR_RESULT
	case 1, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz1:: @ 8162CBE
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz1, MSGBOX_DEFAULT
	multichoice 0, 0, 34, 1
	switch VAR_RESULT
	case 1, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz2:: @ 8162CE1
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz2, MSGBOX_DEFAULT
	multichoice 0, 0, 35, 1
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll4Quiz3:: @ 8162D04
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll4Quiz3, MSGBOX_DEFAULT
	multichoice 0, 0, 36, 1
	switch VAR_RESULT
	case 1, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz1:: @ 8162D27
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz1, MSGBOX_DEFAULT
	multichoice 0, 0, 37, 1
	switch VAR_RESULT
	case 1, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz2:: @ 8162D4A
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz2, MSGBOX_DEFAULT
	multichoice 0, 0, 38, 1
	switch VAR_RESULT
	case 0, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_Mechadoll5Quiz3:: @ 8162D6D
	msgbox Route110_TrickHousePuzzle5_Text_Mechadoll5Quiz3, MSGBOX_DEFAULT
	multichoice 0, 0, 39, 1
	switch VAR_RESULT
	case 2, Route110_TrickHousePuzzle5_EventScript_CorrectAnswer
	goto Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer
	end

Route110_TrickHousePuzzle5_EventScript_IncorrectAnswer:: @ 8162D90
	waitse
	playse SE_FAILURE
	msgbox Route110_TrickHousePuzzle5_Text_DisappointmentError, MSGBOX_DEFAULT
	applymovement 1, Route110_TrickHousePuzzle5_Movement_MechadollShake
	applymovement 2, Route110_TrickHousePuzzle5_Movement_MechadollShake
	applymovement 3, Route110_TrickHousePuzzle5_Movement_MechadollShake
	applymovement 4, Route110_TrickHousePuzzle5_Movement_MechadollShake
	applymovement 5, Route110_TrickHousePuzzle5_Movement_MechadollShake
	msgbox Route110_TrickHousePuzzle5_Text_Wahahahaha, MSGBOX_DEFAULT
	waitmovement 0
	closemessage
	msgbox Route110_TrickHousePuzzle5_Text_WaitForNextChallenge, MSGBOX_DEFAULT
	closemessage
	warp MAP_ROUTE110_TRICK_HOUSE_PUZZLE5, 255, 0, 21
	waitstate
	releaseall
	end

Route110_TrickHousePuzzle5_EventScript_CorrectAnswer:: @ 8162DDF
	waitse
	playse SE_SUCCESS
	goto Route110_TrickHousePuzzle5_EventScript_CorrectGoThrough
	end

Route110_TrickHousePuzzle5_EventScript_CorrectGoThrough:: @ 8162DE9
	msgbox Route110_TrickHousePuzzle5_Text_CorrectGoThrough1, MSGBOX_DEFAULT
	releaseall
	end

Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft1:: @ 8162DF3
	compare VAR_TEMP_8, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkLeft1
	compare VAR_TEMP_8, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkLeft1
	compare VAR_TEMP_8, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkLeft1
	compare VAR_TEMP_8, 4
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkLeft1
	compare VAR_TEMP_8, 5
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkLeft1
	return

Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft2:: @ 8162E2B
	compare VAR_TEMP_8, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkLeft2
	compare VAR_TEMP_8, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkLeft2
	compare VAR_TEMP_8, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkLeft2
	compare VAR_TEMP_8, 4
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkLeft2
	compare VAR_TEMP_8, 5
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkLeft2
	return

Route110_TrickHousePuzzle5_EventScript_MechadollWalkLeft3:: @ 8162E63
	compare VAR_TEMP_8, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkLeft3
	compare VAR_TEMP_8, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkLeft3
	compare VAR_TEMP_8, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkLeft3
	compare VAR_TEMP_9, 4
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkLeft3
	compare VAR_TEMP_9, 5
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkLeft3
	return

Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight1:: @ 8162E9B
	compare VAR_TEMP_8, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkRight1
	compare VAR_TEMP_8, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkRight1
	compare VAR_TEMP_8, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkRight1
	compare VAR_TEMP_8, 4
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkRight1
	compare VAR_TEMP_8, 5
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkRight1
	return

Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight2:: @ 8162ED3
	compare VAR_TEMP_8, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkRight2
	compare VAR_TEMP_8, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkRight2
	compare VAR_TEMP_8, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkRight2
	compare VAR_TEMP_8, 4
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkRight2
	compare VAR_TEMP_8, 5
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkRight2
	return

Route110_TrickHousePuzzle5_EventScript_MechadollWalkRight3:: @ 8162F0B
	compare VAR_TEMP_8, 1
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkRight3
	compare VAR_TEMP_8, 2
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkRight3
	compare VAR_TEMP_8, 3
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkRight3
	compare VAR_TEMP_8, 4
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkRight3
	compare VAR_TEMP_8, 5
	call_if_eq Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkRight3
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkLeft1:: @ 8162F43
	applymovement 1, Route110_TrickHousePuzzle5_Movement_WalkLeft1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkLeft1:: @ 8162F4E
	applymovement 2, Route110_TrickHousePuzzle5_Movement_WalkLeft1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkLeft1:: @ 8162F59
	applymovement 3, Route110_TrickHousePuzzle5_Movement_WalkLeft1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkLeft1:: @ 8162F64
	applymovement 4, Route110_TrickHousePuzzle5_Movement_WalkLeft1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkLeft1:: @ 8162F6F
	applymovement 5, Route110_TrickHousePuzzle5_Movement_WalkLeft1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkLeft2:: @ 8162F7A
	applymovement 1, Route110_TrickHousePuzzle5_Movement_WalkLeft2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkLeft2:: @ 8162F85
	applymovement 2, Route110_TrickHousePuzzle5_Movement_WalkLeft2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkLeft2:: @ 8162F90
	applymovement 3, Route110_TrickHousePuzzle5_Movement_WalkLeft2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkLeft2:: @ 8162F9B
	applymovement 4, Route110_TrickHousePuzzle5_Movement_WalkLeft2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkLeft2:: @ 8162FA6
	applymovement 5, Route110_TrickHousePuzzle5_Movement_WalkLeft2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkLeft3:: @ 8162FB1
	applymovement 1, Route110_TrickHousePuzzle5_Movement_WalkLeft3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkLeft3:: @ 8162FBC
	applymovement 2, Route110_TrickHousePuzzle5_Movement_WalkLeft3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkLeft3:: @ 8162FC7
	applymovement 3, Route110_TrickHousePuzzle5_Movement_WalkLeft3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkLeft3:: @ 8162FD2
	applymovement 4, Route110_TrickHousePuzzle5_Movement_WalkLeft3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkLeft3:: @ 8162FDD
	applymovement 5, Route110_TrickHousePuzzle5_Movement_WalkLeft3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkRight1:: @ 8162FE8
	applymovement 1, Route110_TrickHousePuzzle5_Movement_WalkRight1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkRight1:: @ 8162FF3
	applymovement 2, Route110_TrickHousePuzzle5_Movement_WalkRight1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkRight1:: @ 8162FFE
	applymovement 3, Route110_TrickHousePuzzle5_Movement_WalkRight1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkRight1:: @ 8163009
	applymovement 4, Route110_TrickHousePuzzle5_Movement_WalkRight1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkRight1:: @ 8163014
	applymovement 5, Route110_TrickHousePuzzle5_Movement_WalkRight1
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkRight2:: @ 816301F
	applymovement 1, Route110_TrickHousePuzzle5_Movement_WalkRight2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkRight2:: @ 816302A
	applymovement 2, Route110_TrickHousePuzzle5_Movement_WalkRight2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkRight2:: @ 8163035
	applymovement 3, Route110_TrickHousePuzzle5_Movement_WalkRight2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkRight2:: @ 8163040
	applymovement 4, Route110_TrickHousePuzzle5_Movement_WalkRight2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkRight2:: @ 816304B
	applymovement 5, Route110_TrickHousePuzzle5_Movement_WalkRight2
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll1WalkRight3:: @ 8163056
	applymovement 1, Route110_TrickHousePuzzle5_Movement_WalkRight3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll2WalkRight3:: @ 8163061
	applymovement 2, Route110_TrickHousePuzzle5_Movement_WalkRight3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll3WalkRight3:: @ 816306C
	applymovement 3, Route110_TrickHousePuzzle5_Movement_WalkRight3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll4WalkRight3:: @ 8163077
	applymovement 4, Route110_TrickHousePuzzle5_Movement_WalkRight3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_EventScript_Mechadoll5WalkRight3:: @ 8163082
	applymovement 5, Route110_TrickHousePuzzle5_Movement_WalkRight3
	waitmovement 0
	return

Route110_TrickHousePuzzle5_Movement_WalkLeft1:: @ 816308D
	walk_left
	step_end

Route110_TrickHousePuzzle5_Movement_WalkLeft2:: @ 816308F
	walk_left
	walk_left
	step_end

Route110_TrickHousePuzzle5_Movement_WalkLeft3:: @ 8163092
	walk_left
	walk_left
	walk_left
	step_end

Route110_TrickHousePuzzle5_Movement_WalkRight1:: @ 8163096
	walk_right
	step_end

Route110_TrickHousePuzzle5_Movement_WalkRight2:: @ 8163098
	walk_right
	walk_right
	step_end

Route110_TrickHousePuzzle5_Movement_WalkRight3:: @ 816309B
	walk_right
	walk_right
	walk_right
	step_end

Route110_TrickHousePuzzle5_Movement_MechadollShake:: @ 816309F
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_left
	delay_2
	face_right
	delay_2
	face_down
	step_end
