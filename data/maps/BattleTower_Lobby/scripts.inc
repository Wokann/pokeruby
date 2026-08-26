BattleTower_Lobby_MapScripts:: @ 816027E
	map_script MAP_SCRIPT_ON_RESUME, BattleTower_Lobby_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, BattleTower_Lobby_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, BattleTower_Lobby_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleTower_Lobby_OnWarp
	.byte 0

BattleTower_Lobby_OnResume:: @ 8160293
	special TryEnableBravoTrainerBattleTower
	call BattleTower_Lobby_EventScript_UpdateAwardManVisibility
	end

BattleTower_Lobby_EventScript_UpdateAwardManVisibility:: @ 816029C
	setvar VAR_0x8004, 10
	special SetBattleTowerProperty
	goto_if_set FLAG_RECEIVED_GOLD_SHIELD, BattleTower_Lobby_EventScript_HideAwardMan
	goto_if_set FLAG_RECEIVED_SILVER_SHIELD, BattleTower_Lobby_EventScript_CheckGoldShieldStreak
	specialvar VAR_0x8007, GetBestBattleTowerStreak
	compare VAR_0x8007, 50
	goto_if_lt BattleTower_Lobby_EventScript_HideAwardMan
	clearflag FLAG_HIDE_AWARD_MAN_BATTLE_TOWER
	special TryInitBattleTowerAwardManObjectEvent
	return

BattleTower_Lobby_EventScript_CheckGoldShieldStreak:: @ 81602CD
	specialvar VAR_0x8007, GetBestBattleTowerStreak
	compare VAR_0x8007, 100
	goto_if_lt BattleTower_Lobby_EventScript_HideAwardMan
	clearflag FLAG_HIDE_AWARD_MAN_BATTLE_TOWER
	special TryInitBattleTowerAwardManObjectEvent
	return

BattleTower_Lobby_EventScript_HideAwardMan:: @ 81602E4
	setflag FLAG_HIDE_AWARD_MAN_BATTLE_TOWER
	return

BattleTower_Lobby_OnTransition:: @ 81602E8
	call BattleTower_Lobby_EventScript_ShowOrHideReporter
	end

BattleTower_Lobby_OnWarp:: @ 81602EE
	map_script_2 VAR_TEMP_1, 0, BattleTower_Lobby_EventScript_PlayerFaceNorth
	.2byte 0

BattleTower_Lobby_EventScript_PlayerFaceNorth:: @ 81602F8
	setvar VAR_TEMP_1, 1
	turnobject LOCALID_PLAYER, 2
	end

BattleTower_Lobby_OnFrame:: @ 8160302
	map_script_2 VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 0, BattleTower_Lobby_EventScript_GetChallengeStatus
	map_script_2 VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 1, BattleTower_Lobby_EventScript_QuitWithoutSaving
	map_script_2 VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 2, BattleTower_Lobby_EventScript_WonChallenge
	map_script_2 VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 3, BattleTower_Lobby_EventScript_LostChallenge
	map_script_2 VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 4, BattleTower_Lobby_EventScript_ResumeChallenge
	.2byte 0

BattleTower_Lobby_EventScript_GetChallengeStatus:: @ 816032C
	special GetBattleTowerChallengeStatus
	end

BattleTower_Lobby_EventScript_QuitWithoutSaving:: @ 8160330
	lock
	faceplayer
	msgbox BattleTower_Lobby_Text_DidntSaveBeforeQuitting, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 5
	release
	end

BattleTower_Lobby_EventScript_WonChallenge:: @ 8160342
	lock
	faceplayer
	message BattleTower_Lobby_Text_CongratsBeatenSeven
	waitmessage
	setvar VAR_0x8004, 0
	special BattleTowerUtil
	compare VAR_RESULT, 6
	goto_if_eq BattleTower_Lobby_EventScript_SaveWinRecord
	special AwardBattleTowerRibbons
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_SaveWinRecord
	message BattleTower_Lobby_Text_HereAreSomeRibbons
	waitmessage
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	msgbox BattleTower_Lobby_Text_PutRibbonOnMons, MSGBOX_DEFAULT

BattleTower_Lobby_EventScript_SaveWinRecord:: @ 816037D
	setvar VAR_0x8004, 8
	special BattleTowerUtil
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Lobby_EventScript_GivePrize
	message BattleTower_Lobby_Text_RecordWillBeSaved
	waitmessage
	setvar VAR_0x8004, 13
	special SetBattleTowerProperty
	special DetermineBattleTowerPrize
	setvar VAR_0x8004, 3
	special SaveBattleTowerProgress
	playse SE_SAVE
	waitse

BattleTower_Lobby_EventScript_GivePrize:: @ 81603AD
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, 3
	special SetBattleTowerProperty
	message BattleTower_Lobby_Text_EarnedFabulousPrize
	waitmessage
	special GiveBattleTowerPrize
	switch VAR_RESULT
	case 0, BattleTower_Lobby_EventScript_BagFullMakeRoom
	message BattleTower_Lobby_Text_ReceivedPrize
	waitmessage
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	setvar VAR_0x8004, 9
	special BattleTowerUtil
	compare VAR_RESULT, 49
	goto_if_ne BattleTower_Lobby_EventScript_LookForwardToChallenge
	msgbox BattleTower_Lobby_Text_AboutToFace50thTrainer, MSGBOX_DEFAULT

BattleTower_Lobby_EventScript_LookForwardToChallenge:: @ 81603F8
	msgbox BattleTower_Lobby_Text_LookForwardToAnotherChallenge, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 5
	release
	end

BattleTower_Lobby_EventScript_BagFullMakeRoom:: @ 8160408
	msgbox BattleTower_Lobby_Text_BagFullMakeRoom, MSGBOX_DEFAULT
	waitmessage
	closemessage
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 5
	release
	end

BattleTower_Lobby_EventScript_LostChallenge:: @ 8160419
	lock
	faceplayer
	message BattleTower_Lobby_Text_ThankYouForPlaying
	waitmessage
	message BattleTower_Lobby_Text_RecordWillBeSaved
	waitmessage
	setvar VAR_0x8004, 13
	special SetBattleTowerProperty
	setvar VAR_0x8004, 0
	special SaveBattleTowerProgress
	playse SE_SAVE
	waitse
	msgbox BattleTower_Lobby_Text_LookForwardToAnotherChallenge, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 5
	release
	end

BattleTower_Lobby_EventScript_ResumeChallenge:: @ 816044B
	lock
	faceplayer
	setvar VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 1
	message BattleTower_Lobby_Text_WeveBeenWaitingForYou
	waitmessage
	message BattleTower_Lobby_Text_ProgressWillBeSaved
	waitmessage
	setvar VAR_0x8004, 1
	special SaveBattleTowerProgress
	playse SE_SAVE
	waitse
	setvar VAR_0x8004, 8
	setvar VAR_0x8005, 0
	special SetBattleTowerProperty
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 5
	special SavePlayerParty
	special SetBattleTowerParty
	msgbox BattleTower_Lobby_Text_DirectYouToBattleRoom, MSGBOX_DEFAULT
	closemessage
	call BattleTower_Lobby_EventScript_EnterElevator
	warp MAP_BATTLE_TOWER_ELEVATOR, 255, 1, 6
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 0
	waitstate

BattleTower_Lobby_EventScript_SinglesAttendant:: @ 816049E
	lock
	faceplayer
	setvar VAR_0x8004, 0
	special BattleTowerUtil
	compare VAR_RESULT, 6
	goto_if_eq BattleTower_Lobby_EventScript_WonChallenge
	special SavePlayerParty
	msgbox BattleTower_Lobby_Text_WelcomeToBattleTower, MSGBOX_DEFAULT

BattleTower_Lobby_EventScript_AskEnterSinglesChallenge:: @ 81604BE
	message BattleTower_Lobby_Text_TakeSinglesChallenge
	waitmessage
	multichoice 15, 6, 23, 0
	switch VAR_RESULT
	case 0, BattleTower_Lobby_EventScript_TryEnterSinglesChallenge
	case 1, BattleTower_Lobby_EventScript_ExplainSinglesChallenge
	case 2, BattleTower_Lobby_EventScript_CancelChallenge
	case 127, BattleTower_Lobby_EventScript_CancelChallenge

BattleTower_Lobby_EventScript_TryEnterSinglesChallenge:: @ 81604FA
	message BattleTower_Lobby_Text_WhichLevelMode
	waitmessage
	multichoice 18, 6, 24, 0
	switch VAR_RESULT
	case 2, BattleTower_Lobby_EventScript_CancelChallenge
	case 127, BattleTower_Lobby_EventScript_CancelChallenge
	special CheckPartyBattleTowerBanlist
	compare VAR_0x8004, 1
	goto_if_eq BattleTower_Lobby_EventScript_NotEnoughValidMons
	setvar VAR_0x8004, 1
	copyvar VAR_0x8005, VAR_RESULT
	copyvar VAR_0x8006, VAR_RESULT
	special SetBattleTowerProperty
	msgbox BattleTower_Lobby_Text_SelectThreeMons, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	special ChooseBattleTowerPlayerParty
	waitstate
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_LoadPartyCancelChallenge
	msgbox BattleTower_Lobby_Text_OkayToSaveBeforeEntering, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, BattleTower_Lobby_EventScript_LoadPartyCancelChallenge
	case YES, BattleTower_Lobby_EventScript_SaveBeforeSinglesChallenge
	case 127, BattleTower_Lobby_EventScript_LoadPartyCancelChallenge

BattleTower_Lobby_EventScript_SaveBeforeSinglesChallenge:: @ 8160587
	setvar VAR_0x8004, 5
	special SetBattleTowerProperty
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 0
	setvar VAR_0x8004, 11
	special SetBattleTowerProperty
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, 1
	special SetBattleTowerProperty
	setvar VAR_0x8004, 8
	setvar VAR_0x8005, 0
	special SetBattleTowerProperty
	special LoadPlayerParty
	closemessage
	delay 2
	call Common_EventScript_SaveGame
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 5
	erasebox 0, 0, 15, 10
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_CancelChallengeSaveFailed
	incrementgamestat GAME_STAT_ENTERED_BATTLE_TOWER
	special SavePlayerParty
	special SetBattleTowerParty
	setvar VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 1
	msgbox BattleTower_Lobby_Text_DirectYouToBattleRoom, MSGBOX_DEFAULT
	closemessage
	call BattleTower_Lobby_EventScript_EnterElevator
	warp MAP_BATTLE_TOWER_ELEVATOR, 255, 1, 6
	setvar VAR_TEMP_BATTLE_TOWER_CHALLENGE_STATUS, 0
	waitstate

BattleTower_Lobby_EventScript_ExplainSinglesChallenge:: @ 8160600
	msgbox BattleTower_Lobby_Text_ExplainSinglesChallenge, MSGBOX_DEFAULT
	goto BattleTower_Lobby_EventScript_AskEnterSinglesChallenge

BattleTower_Lobby_EventScript_NotEnoughValidMons:: @ 816060D
	switch VAR_RESULT
	case 0, BattleTower_Lobby_EventScript_NotEnoughValidMonsLv50
	case 1, BattleTower_Lobby_EventScript_NotEnoughValidMonsLv100

BattleTower_Lobby_EventScript_NotEnoughValidMonsLv50:: @ 8160628
	msgbox BattleTower_Lobby_Text_NotEnoughValidMonsLv50, MSGBOX_DEFAULT
	goto BattleTower_Lobby_EventScript_EndCancelChallenge

BattleTower_Lobby_EventScript_NotEnoughValidMonsLv100:: @ 8160635
	msgbox BattleTower_Lobby_Text_NotEnoughValidMonsLv100, MSGBOX_DEFAULT
	goto BattleTower_Lobby_EventScript_EndCancelChallenge

BattleTower_Lobby_EventScript_CancelChallengeSaveFailed:: @ 8160642
	setvar VAR_0x8004, 12
	special SetBattleTowerProperty
	goto BattleTower_Lobby_EventScript_CancelChallenge

BattleTower_Lobby_EventScript_LoadPartyCancelChallenge:: @ 816064F
	special LoadPlayerParty

BattleTower_Lobby_EventScript_CancelChallenge:: @ 8160652
	msgbox BattleTower_Lobby_Text_LookForwardToAnotherChallenge, MSGBOX_DEFAULT
	setvar VAR_0x8004, 11
	special SetBattleTowerProperty

BattleTower_Lobby_EventScript_EndCancelChallenge:: @ 8160662
	release
	end

BattleTower_Lobby_EventScript_EnterElevator:: @ 8160664
	applymovement 1, BattleTower_Lobby_Movement_WalkToElevator
	applymovement LOCALID_PLAYER, BattleTower_Lobby_Movement_WalkToElevator
	waitmovement 0
	opendoor 6, 1
	waitdooranim
	applymovement 1, BattleTower_Lobby_Movement_AttendantEnterElevator
	applymovement LOCALID_PLAYER, BattleTower_Lobby_Movement_PlayerEnterElevator
	waitmovement 0
	closedoor 6, 1
	waitdooranim
	return

BattleTower_Lobby_Movement_WalkToElevator:: @ 8160693
	walk_up
	walk_up
	walk_up
	step_end

BattleTower_Lobby_Movement_AttendantEnterElevator:: @ 8160697
	walk_up
	set_invisible
	step_end

BattleTower_Lobby_Movement_PlayerEnterElevator:: @ 816069A
	walk_up
	walk_up
	set_invisible
	step_end

@ Unused
BattleTower_Lobby_Movement_UnusedEnterElevator:: @ 816069E
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	set_invisible
	step_end

BattleTower_Lobby_EventScript_FeelingsMan:: @ 81606A5
	lock
	faceplayer
	msgbox BattleTower_Lobby_Text_DescribeFeelingsBeforeBattle, MSGBOX_YESNO
	switch VAR_RESULT
	case YES, BattleTower_Lobby_EventScript_FeelingsBeforeBattle
	case NO, BattleTower_Lobby_EventScript_FeelingsWontTell
	release
	end

BattleTower_Lobby_EventScript_FeelingsBeforeBattle:: @ 81606CC
	msgbox BattleTower_Lobby_Text_WhatAreYourThoughts, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1
	call Common_EventScript_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_CanceledEasyChat
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Lobby_EventScript_SubmittedFeelings
	end

BattleTower_Lobby_EventScript_FeelingsWontTell:: @ 81606F7
	msgbox BattleTower_Lobby_Text_DontThinkMuchAboutIt, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_CanceledEasyChat:: @ 8160701
	msgbox BattleTower_Lobby_Text_ChangedYourMind, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_SubmittedFeelings:: @ 816070B
	msgbox BattleTower_Lobby_Text_ThatsHowYouFeel, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_Woman:: @ 8160715
	msgbox BattleTower_Lobby_Text_WinsInRowRecorded, MSGBOX_NPC
	end

BattleTower_Lobby_EventScript_Boy:: @ 816071E
	msgbox BattleTower_Lobby_Text_CanLeaveUntilLossOrSevenWins, MSGBOX_NPC
	end

BattleTower_Lobby_EventScript_ShowSinglesResults:: @ 8160727
	lockall
	special ShowBattleTowerRecords
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

BattleTower_Lobby_EventScript_AwardMan:: @ 8160733
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_GOLD_SHIELD, BattleTower_Lobby_EventScript_AwardManAlreadyGaveShields
	goto_if_set FLAG_RECEIVED_SILVER_SHIELD, BattleTower_Lobby_EventScript_GiveGoldShield
	msgbox BattleTower_Lobby_Text_AwardSilverShield, MSGBOX_DEFAULT
	setvar VAR_0x8008, DECOR_SILVER_SHIELD
	givedecoration DECOR_SILVER_SHIELD
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_NoRoomForShield
	setflag FLAG_RECEIVED_SILVER_SHIELD
	msgbox BattleTower_Lobby_Text_KeepShieldDisplayed, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_GiveGoldShield:: @ 8160773
	specialvar VAR_RESULT, GetBestBattleTowerStreak
	compare VAR_RESULT, 100
	goto_if_lt BattleTower_Lobby_EventScript_AwardManAlreadyGaveShields
	msgbox BattleTower_Lobby_Text_AwardGoldShield, MSGBOX_DEFAULT
	setvar VAR_0x8008, DECOR_GOLD_SHIELD
	givedecoration DECOR_GOLD_SHIELD
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_NoRoomForShield
	setflag FLAG_RECEIVED_GOLD_SHIELD
	msgbox BattleTower_Lobby_Text_KeepShieldDisplayed, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_NoRoomForShield:: @ 81607AF
	bufferdecorationname 1, VAR_0x8008
	msgbox Text_NoRoomLeftForAnother, MSGBOX_DEFAULT
	msgbox BattleTower_Lobby_Text_NoRoomForShield, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_AwardManAlreadyGaveShields:: @ 81607C5
	msgbox BattleTower_Lobby_Text_SatisfiedWithPerformance, MSGBOX_DEFAULT
	release
	end
