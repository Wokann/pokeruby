BattleTower_Lobby_MapScripts:: @ 816027E
	map_script MAP_SCRIPT_ON_RESUME, BattleTower_Lobby_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, BattleTower_Lobby_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, BattleTower_Lobby_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleTower_Lobby_OnWarp
	.byte 0

BattleTower_Lobby_OnResume:: @ 8160293
	special TryHideBattleTowerReporter
	call BattleTower_Lobby_EventScript_CheckAwardMan
	end

BattleTower_Lobby_EventScript_CheckAwardMan:: @ 816029C
	setvar VAR_0x8004, 10
	special SetBattleTowerProperty
	goto_if_set FLAG_RECEIVED_GOLD_SHIELD, BattleTower_Lobby_EventScript_HideAwardMan
	goto_if_set FLAG_RECEIVED_SILVER_SHIELD, BattleTower_Lobby_EventScript_CheckGoldShieldAward
	specialvar VAR_0x8007, GetBattleTowerSinglesStreak
	compare VAR_0x8007, 50
	goto_if_lt BattleTower_Lobby_EventScript_HideAwardMan
	clearflag FLAG_HIDE_AWARD_MAN_BATTLE_TOWER
	special TryInitBattleTowerAwardManObjectEvent
	return

BattleTower_Lobby_EventScript_CheckGoldShieldAward:: @ 81602CD
	specialvar VAR_0x8007, GetBattleTowerSinglesStreak
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
	map_script_2 VAR_TEMP_1, 0, BattleTower_Lobby_EventScript_TurnPlayerSouth
	.2byte 0

BattleTower_Lobby_EventScript_TurnPlayerSouth:: @ 81602F8
	setvar VAR_TEMP_1, 1
	turnobject LOCALID_PLAYER, 2
	end

BattleTower_Lobby_OnFrame:: @ 8160302
	map_script_2 VAR_TEMP_0, 0, BattleTower_Lobby_EventScript_SetLobbyState
	map_script_2 VAR_TEMP_0, 1, BattleTower_Lobby_EventScript_DisqualifyUnsavedChallenge
	map_script_2 VAR_TEMP_0, 2, BattleTower_Lobby_EventScript_AwardSevenWins
	map_script_2 VAR_TEMP_0, 3, BattleTower_Lobby_EventScript_FinishChallengeAfterLoss
	map_script_2 VAR_TEMP_0, 4, BattleTower_Lobby_EventScript_ResumeChallenge
	.2byte 0

BattleTower_Lobby_EventScript_SetLobbyState:: @ 816032C
	special SetBattleTowerLobbyState
	end

BattleTower_Lobby_EventScript_DisqualifyUnsavedChallenge:: @ 8160330
	lock
	faceplayer
	msgbox BattleTower_Lobby_Text_UnsavedChallengeDisqualified, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_0, 5
	release
	end

BattleTower_Lobby_EventScript_AwardSevenWins:: @ 8160342
	lock
	faceplayer
	message BattleTower_Lobby_Text_BeatSevenTrainers
	waitmessage
	setvar VAR_0x8004, 0
	special CallBattleTowerFunc
	compare VAR_RESULT, 6
	goto_if_eq BattleTower_Lobby_EventScript_CheckPrize
	special AwardBattleTowerRibbons
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_CheckPrize
	message BattleTower_Lobby_Text_ReceivedRibbons
	waitmessage
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	msgbox BattleTower_Lobby_Text_PutRibbonsOnPokemon, MSGBOX_DEFAULT

BattleTower_Lobby_EventScript_CheckPrize:: @ 816037D
	setvar VAR_0x8004, 8
	special CallBattleTowerFunc
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Lobby_EventScript_GivePrize
	message BattleTower_Lobby_Text_SavingRecord
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
	message BattleTower_Lobby_Text_EarnedPrize
	waitmessage
	special GiveBattleTowerPrize
	switch VAR_RESULT
	case 0, BattleTower_Lobby_EventScript_PrizeBagFull
	message BattleTower_Lobby_Text_ReceivedPrize
	waitmessage
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	setvar VAR_0x8004, 9
	special CallBattleTowerFunc
	compare VAR_RESULT, 49
	goto_if_ne BattleTower_Lobby_EventScript_FinishChallenge
	msgbox BattleTower_Lobby_Text_AboutToFace50thTrainer, MSGBOX_DEFAULT

BattleTower_Lobby_EventScript_FinishChallenge:: @ 81603F8
	msgbox BattleTower_Lobby_Text_LookingForwardToNextChallenge, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_0, 5
	release
	end

BattleTower_Lobby_EventScript_PrizeBagFull:: @ 8160408
	msgbox BattleTower_Lobby_Text_BagFullForPrize, MSGBOX_DEFAULT
	waitmessage
	closemessage
	setvar VAR_TEMP_0, 5
	release
	end

BattleTower_Lobby_EventScript_FinishChallengeAfterLoss:: @ 8160419
	lock
	faceplayer
	message BattleTower_Lobby_Text_ThankYouForPlaying
	waitmessage
	message BattleTower_Lobby_Text_SavingRecord
	waitmessage
	setvar VAR_0x8004, 13
	special SetBattleTowerProperty
	setvar VAR_0x8004, 0
	special SaveBattleTowerProgress
	playse SE_SAVE
	waitse
	msgbox BattleTower_Lobby_Text_LookingForwardToNextChallenge, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_0, 5
	release
	end

BattleTower_Lobby_EventScript_ResumeChallenge:: @ 816044B
	lock
	faceplayer
	setvar VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 1
	message BattleTower_Lobby_Text_WelcomeBack
	waitmessage
	message BattleTower_Lobby_Text_SavingBeforeEntry
	waitmessage
	setvar VAR_0x8004, 1
	special SaveBattleTowerProgress
	playse SE_SAVE
	waitse
	setvar VAR_0x8004, 8
	setvar VAR_0x8005, 0
	special SetBattleTowerProperty
	setvar VAR_TEMP_0, 5
	special SavePlayerParty
	special SetBattleTowerParty
	msgbox BattleTower_Lobby_Text_DirectYouToBattleRoom, MSGBOX_DEFAULT
	closemessage
	call BattleTower_Lobby_EventScript_EnterElevator
	warp MAP_BATTLE_TOWER_ELEVATOR, 255, 1, 6
	setvar VAR_TEMP_0, 0
	waitstate

BattleTower_Lobby_EventScript_Receptionist:: @ 816049E
	lock
	faceplayer
	setvar VAR_0x8004, 0
	special CallBattleTowerFunc
	compare VAR_RESULT, 6
	goto_if_eq BattleTower_Lobby_EventScript_AwardSevenWins
	special SavePlayerParty
	msgbox BattleTower_Lobby_Text_WelcomeRegisterHere, MSGBOX_DEFAULT

BattleTower_Lobby_EventScript_ShowChallengeMenu:: @ 81604BE
	message BattleTower_Lobby_Text_MakeBattleRoomChallenge
	waitmessage
	multichoice 15, 6, 23, 0
	switch VAR_RESULT
	case 0, BattleTower_Lobby_EventScript_ChooseLevel
	case 1, BattleTower_Lobby_EventScript_ExplainBattleTower
	case 2, BattleTower_Lobby_EventScript_CancelRegistration
	case 127, BattleTower_Lobby_EventScript_CancelRegistration

BattleTower_Lobby_EventScript_ChooseLevel:: @ 81604FA
	message BattleTower_Lobby_Text_ChooseLevel50Or100
	waitmessage
	multichoice 18, 6, 24, 0
	switch VAR_RESULT
	case 2, BattleTower_Lobby_EventScript_CancelRegistration
	case 127, BattleTower_Lobby_EventScript_CancelRegistration
	special CheckPartyBattleTowerBanlist
	compare VAR_0x8004, 1
	goto_if_eq BattleTower_Lobby_EventScript_ShowEligibilityError
	setvar VAR_0x8004, 1
	copyvar VAR_0x8005, VAR_RESULT
	copyvar VAR_0x8006, VAR_RESULT
	special SetBattleTowerProperty
	msgbox BattleTower_Lobby_Text_ChoosePokemon, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	special ChoosePartyForBattleTower
	waitstate
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_RestoreParty
	msgbox BattleTower_Lobby_Text_SaveBeforeEntering, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, BattleTower_Lobby_EventScript_RestoreParty
	case YES, BattleTower_Lobby_EventScript_ConfirmChallengeAndSave
	case 127, BattleTower_Lobby_EventScript_RestoreParty

BattleTower_Lobby_EventScript_ConfirmChallengeAndSave:: @ 8160587
	setvar VAR_0x8004, 5
	special SetBattleTowerProperty
	setvar VAR_TEMP_0, 0
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
	setvar VAR_TEMP_0, 5
	erasebox 0, 0, 15, 10
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_SaveFailed
	incrementgamestat GAME_STAT_ENTERED_BATTLE_TOWER
	special SavePlayerParty
	special SetBattleTowerParty
	setvar VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 1
	msgbox BattleTower_Lobby_Text_DirectYouToBattleRoom, MSGBOX_DEFAULT
	closemessage
	call BattleTower_Lobby_EventScript_EnterElevator
	warp MAP_BATTLE_TOWER_ELEVATOR, 255, 1, 6
	setvar VAR_TEMP_0, 0
	waitstate

BattleTower_Lobby_EventScript_ExplainBattleTower:: @ 8160600
	msgbox BattleTower_Lobby_Text_ExplainBattleTower, MSGBOX_DEFAULT
	goto BattleTower_Lobby_EventScript_ShowChallengeMenu

BattleTower_Lobby_EventScript_ShowEligibilityError:: @ 816060D
	switch VAR_RESULT
	case 0, BattleTower_Lobby_EventScript_NotEnoughLevel50Mons
	case 1, BattleTower_Lobby_EventScript_NotEnoughLevel100Mons

BattleTower_Lobby_EventScript_NotEnoughLevel50Mons:: @ 8160628
	msgbox BattleTower_Lobby_Text_NeedThreeEligibleLevel50Mons, MSGBOX_DEFAULT
	goto BattleTower_Lobby_EventScript_ReleaseReceptionist

BattleTower_Lobby_EventScript_NotEnoughLevel100Mons:: @ 8160635
	msgbox BattleTower_Lobby_Text_NeedThreeEligibleLevel100Mons, MSGBOX_DEFAULT
	goto BattleTower_Lobby_EventScript_ReleaseReceptionist

BattleTower_Lobby_EventScript_SaveFailed:: @ 8160642
	setvar VAR_0x8004, 12
	special SetBattleTowerProperty
	goto BattleTower_Lobby_EventScript_CancelRegistration

BattleTower_Lobby_EventScript_RestoreParty:: @ 816064F
	special LoadPlayerParty

BattleTower_Lobby_EventScript_CancelRegistration:: @ 8160652
	msgbox BattleTower_Lobby_Text_LookingForwardToNextChallenge, MSGBOX_DEFAULT
	setvar VAR_0x8004, 11
	special SetBattleTowerProperty

BattleTower_Lobby_EventScript_ReleaseReceptionist:: @ 8160662
	release
	end

BattleTower_Lobby_EventScript_EnterElevator:: @ 8160664
	applymovement 1, BattleTower_Lobby_Movement_ApproachElevatorDoor
	applymovement LOCALID_PLAYER, BattleTower_Lobby_Movement_ApproachElevatorDoor
	waitmovement 0
	opendoor 6, 1
	waitdooranim
	applymovement 1, BattleTower_Lobby_Movement_AttendantEnterElevator
	applymovement LOCALID_PLAYER, BattleTower_Lobby_Movement_PlayerEnterElevator
	waitmovement 0
	closedoor 6, 1
	waitdooranim
	return

BattleTower_Lobby_Movement_ApproachElevatorDoor:: @ 8160693
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

@ 816069E
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	set_invisible
	step_end

BattleTower_Lobby_EventScript_AskForInterview:: @ 81606A5
	lock
	faceplayer
	msgbox BattleTower_Lobby_Text_ReporterAskThoughts, MSGBOX_YESNO
	switch VAR_RESULT
	case YES, BattleTower_Lobby_EventScript_ReporterAskEasyChat
	case NO, BattleTower_Lobby_EventScript_ReporterDeclined
	release
	end

BattleTower_Lobby_EventScript_ReporterAskEasyChat:: @ 81606CC
	msgbox BattleTower_Lobby_Text_ReporterAskAnswer, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_ReporterCanceled
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Lobby_EventScript_ReporterAccepted
	end

BattleTower_Lobby_EventScript_ReporterDeclined:: @ 81606F7
	msgbox BattleTower_Lobby_Text_ReporterDeclined, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_ReporterCanceled:: @ 8160701
	msgbox BattleTower_Lobby_Text_ReporterCanceled, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_ReporterAccepted:: @ 816070B
	msgbox BattleTower_Lobby_Text_ReporterAccepted, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_VisitorRecords:: @ 8160715
	msgbox BattleTower_Lobby_Text_ExplainWinRecord, MSGBOX_NPC
	end

BattleTower_Lobby_EventScript_VisitorChallenge:: @ 816071E
	msgbox BattleTower_Lobby_Text_ExplainSevenTrainerChallenge, MSGBOX_NPC
	end

BattleTower_Lobby_EventScript_RecordsBoard:: @ 8160727
	lockall
	special ShowBattleTowerRecords
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

BattleTower_Lobby_EventScript_AwardMan:: @ 8160733
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_GOLD_SHIELD, BattleTower_Lobby_EventScript_AlreadyAwarded
	goto_if_set FLAG_RECEIVED_SILVER_SHIELD, BattleTower_Lobby_EventScript_GiveGoldShield
	msgbox BattleTower_Lobby_Text_OwnerSilverShieldAward, MSGBOX_DEFAULT
	setvar VAR_0x8008, DECOR_SILVER_SHIELD
	givedecoration DECOR_SILVER_SHIELD
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_DecorationStorageFull
	setflag FLAG_RECEIVED_SILVER_SHIELD
	msgbox BattleTower_Lobby_Text_DisplayShield, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_GiveGoldShield:: @ 8160773
	specialvar VAR_RESULT, GetBattleTowerSinglesStreak
	compare VAR_RESULT, 100
	goto_if_lt BattleTower_Lobby_EventScript_AlreadyAwarded
	msgbox BattleTower_Lobby_Text_OwnerGoldShieldAward, MSGBOX_DEFAULT
	setvar VAR_0x8008, DECOR_GOLD_SHIELD
	givedecoration DECOR_GOLD_SHIELD
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Lobby_EventScript_DecorationStorageFull
	setflag FLAG_RECEIVED_GOLD_SHIELD
	msgbox BattleTower_Lobby_Text_DisplayShield, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_DecorationStorageFull:: @ 81607AF
	bufferdecorationname 1, VAR_0x8008
	msgbox Text_NoRoomLeftForAnother, MSGBOX_DEFAULT
	msgbox BattleTower_Lobby_Text_NoRoomForShield, MSGBOX_DEFAULT
	release
	end

BattleTower_Lobby_EventScript_AlreadyAwarded:: @ 81607C5
	msgbox BattleTower_Lobby_Text_OwnerAlreadyAwarded, MSGBOX_DEFAULT
	release
	end
