CableClub_OnWarp:: @ 81A3D03
	map_script_2 VAR_CABLE_CLUB_STATE, 1, CableClub_EventScript_CheckTurnAttendant
	map_script_2 VAR_CABLE_CLUB_STATE, 2, CableClub_EventScript_CheckTurnAttendant
	map_script_2 VAR_CABLE_CLUB_STATE, 5, CableClub_EventScript_CheckTurnAttendant
	map_script_2 VAR_CABLE_CLUB_STATE, 3, CableClub_EventScript_CheckTurnAttendant
	map_script_2 VAR_CABLE_CLUB_STATE, 4, CableClub_EventScript_CheckTurnAttendant
	.2byte 0

CableClub_EventScript_CheckTurnAttendant:: @ 81A3D2D
	turnobject VAR_0x8007, 4
	end

CableClub_OnLoad:: @ 81A3D32
	compare VAR_CABLE_CLUB_STATE, 1
	goto_if_eq CableClub_EventScript_OnLoadFromColosseum
	compare VAR_CABLE_CLUB_STATE, 2
	goto_if_eq CableClub_EventScript_OnLoadFromColosseum
	compare VAR_CABLE_CLUB_STATE, 5
	goto_if_eq CableClub_EventScript_OnLoadFromColosseum
	compare VAR_CABLE_CLUB_STATE, 3
	goto_if_eq CableClub_EventScript_OnLoadFromTradeCenter
	compare VAR_CABLE_CLUB_STATE, 4
	goto_if_eq CableClub_EventScript_OnLoadFromRecordCorner
	end

CableClub_EventScript_OnLoadFromColosseum:: @ 81A3D6A
	setmetatile 5, 3, METATILE_PokemonCenter_Floor_Plain_Alt, 0
	end

CableClub_EventScript_OnLoadFromTradeCenter:: @ 81A3D74
	setmetatile 8, 3, METATILE_PokemonCenter_Floor_Plain_Alt, 0
	end

CableClub_EventScript_OnLoadFromRecordCorner:: @ 81A3D7E
	setmetatile 11, 3, METATILE_PokemonCenter_Floor_Plain_Alt, 0
	end

CableClub_OnFrame:: @ 81A3D88
	map_script_2 VAR_CABLE_CLUB_STATE, 1, CableClub_EventScript_ExitLinkRoom
	map_script_2 VAR_CABLE_CLUB_STATE, 2, CableClub_EventScript_ExitLinkRoom
	map_script_2 VAR_CABLE_CLUB_STATE, 5, CableClub_EventScript_ExitLinkRoom
	map_script_2 VAR_CABLE_CLUB_STATE, 3, CableClub_EventScript_ExitTradeCenter
	map_script_2 VAR_CABLE_CLUB_STATE, 4, CableClub_EventScript_ExitRecordCorner
	.2byte 0

CableClub_EventScript_PlayerExitRecordCorner:: @ 81A3DB2
	special CloseLink
	setvar VAR_CABLE_CLUB_STATE, 0
	applymovement LOCALID_PLAYER, Movement_PlayerExitLinkRoom
	waitmovement 0
	compare VAR_0x8007, 0
	goto_if_eq CableClub_EventScript_ExitRecordCornerRet
	applymovement VAR_0x8007, Movement_AttendantFaceDown
	waitmovement 0

CableClub_EventScript_ExitRecordCornerRet:: @ 81A3DD9
	return

CableClub_EventScript_CloseLinkAndExitLinkRoom:: @ 81A3DDA
	special CloseLink
	setvar VAR_CABLE_CLUB_STATE, 0
	compare VAR_0x8007, 0
	goto_if_eq CableClub_EventScript_PlayerExitLinkRoom
	applymovement VAR_0x8007, Movement_AttendantFaceRight
	waitmovement 0
	applymovement LOCALID_PLAYER, Movement_PlayerExitLinkRoom
	waitmovement 0
	applymovement VAR_0x8007, Movement_AttendantFaceDown
	waitmovement 0
	return

CableClub_EventScript_PlayerExitLinkRoom:: @ 81A3E0C
	applymovement LOCALID_PLAYER, Movement_PlayerExitLinkRoom
	waitmovement 0
	return

CableClub_EventScript_ExitTradeCenter:: @ 81A3E17
	lockall
	call CableClub_EventScript_PlayerExitTradeCenter
	setmetatile 8, 3, METATILE_PokemonCenter_CounterBarrier, 1
	special DrawWholeMapView
	erasebox 0, 0, 29, 19
	releaseall
	end

CableClub_EventScript_PlayerExitTradeCenter:: @ 81A3E30
	special CloseLink
	setvar VAR_CABLE_CLUB_STATE, 0
	compare VAR_0x8007, 0
	goto_if_eq CableClub_EventScript_PlayerExitLinkRoom
	applymovement LOCALID_PLAYER, Movement_PlayerFaceAttendantLeft
	waitmovement 0
	applymovement VAR_0x8007, Movement_AttendantFaceRight
	waitmovement 0
	message CableClub_Text_TrainerCardDataOverwritten
	waitmessage
	playse SE_PIN
	message CableClub_Text_HopeToSeeYouAgain
	waitmessage
	applymovement LOCALID_PLAYER, Movement_PlayerExitLinkRoom
	waitmovement 0
	applymovement VAR_0x8007, Movement_AttendantFaceDown
	waitmovement 0
	return

CableClub_EventScript_ExitRecordCorner:: @ 81A3E7B
	call CableClub_EventScript_PlayerExitRecordCorner
	setmetatile 11, 3, METATILE_PokemonCenter_CounterBarrier, 1
	special DrawWholeMapView
	end

CableClub_EventScript_ExitLinkRoom:: @ 81A3E8D
	lockall
	call CableClub_EventScript_CloseLinkAndExitLinkRoom
	setmetatile 5, 3, METATILE_PokemonCenter_CounterBarrier, 1
	special DrawWholeMapView
	erasebox 0, 0, 29, 19
	releaseall
	end

CableClub_EventScript_Colosseum:: @ 81A3EA6
	goto_if_unset FLAG_SYS_POKEDEX_GET, CableClub_EventScript_ColosseumUndergoingAdjustments
	copyvar VAR_0x8007, VAR_LAST_TALKED
	lock
	faceplayer
	message CableClub_Text_WelcomeToColosseum
	waitmessage

CableClub_EventScript_SelectColosseumService:: @ 81A3EBC
	multichoice 17, 6, 17, 0
	switch VAR_RESULT
	case 0, CableClub_EventScript_TryEnterColosseum
	case 1, CableClub_EventScript_AbortLink
	case 2, CableClub_EventScript_BattleModeInfo
	case 127, CableClub_EventScript_AbortLink

CableClub_EventScript_TryEnterColosseum:: @ 81A3EF2
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq CableClub_EventScript_AbortLink
	call CableClub_EventScript_SelectBattleMode
	compare VAR_0x8004, 0
	goto_if_eq CableClub_EventScript_AbortLink
	message gText_PleaseWaitForLink
	waitmessage
	special TryBattleLinkup
	waitstate
	compare VAR_RESULT, 1
	goto_if_eq CableClub_EventScript_EnterColosseum
	compare VAR_RESULT, 2
	goto_if_eq CableClub_EventScript_AbortLinkSomeoneNotReady
	compare VAR_RESULT, 3
	goto_if_eq CableClub_EventScript_AbortLinkDifferentSelections
	compare VAR_RESULT, 4
	goto_if_eq CableClub_EventScript_AbortLinkIncorrectNumberOfBattlers
	compare VAR_RESULT, 5
	goto_if_eq CableClub_EventScript_AbortLink
	compare VAR_RESULT, 6
	goto_if_eq CableClub_EventScript_AbortLinkConnectionError

CableClub_EventScript_EnterColosseum:: @ 81A3F5E
	special ScrSpecial_HealPlayerParty
	special SavePlayerParty
	special LoadPlayerBag
	copyvar VAR_CABLE_CLUB_STATE, VAR_0x8004
	messageautoscroll CableClub_Text_PleaseEnter
	waitmessage
	setmetatile 5, 3, METATILE_PokemonCenter_Floor_Plain_Alt, 0
	special DrawWholeMapView
	delay 60
	applymovement VAR_LAST_TALKED, Movement_AttendantFaceRight
	waitmovement 0
	closemessage
	applymovement LOCALID_PLAYER, Movement_PlayerApproachLinkRoomRight
	waitmovement 0
	opendoor 5, 1
	waitdooranim
	applymovement LOCALID_PLAYER, Movement_PlayerEnterLinkRoom
	waitmovement 0
	hideobjectat LOCALID_PLAYER, MAP_PETALBURG_CITY
	closedoor 5, 1
	waitdooranim
	release
	compare VAR_0x8004, 5
	goto_if_eq CableClub_EventScript_WarpTo4PColosseum
	special SetCableClubWarp
	warp MAP_BATTLE_COLOSSEUM_2P, 255, 6, 8
	special DoCableClubWarp
	waitstate
	end

CableClub_EventScript_WarpTo4PColosseum:: @ 81A3FCD
	special SetCableClubWarp
	warp MAP_BATTLE_COLOSSEUM_4P, 255, 5, 8
	special DoCableClubWarp
	waitstate
	end

CableClub_EventScript_BattleModeInfo:: @ 81A3FDD
	message CableClub_Text_ExplainBattleModes
	waitmessage
	waitbuttonpress
	message CableClub_Text_WouldYouLikeToBattle
	waitmessage
	goto CableClub_EventScript_SelectColosseumService

CableClub_EventScript_SelectBattleMode:: @ 81A3FEF
	message CableClub_Text_PlayWhichBattleMode
	waitmessage
	multichoice 0, 0, 18, 0
	switch VAR_RESULT
	case 0, CableClub_EventScript_SingleBattleMode
	case 1, CableClub_EventScript_DoubleBattleMode
	case 2, CableClub_EventScript_MultiBattleMode
	case 3, CableClub_EventScript_CancelBattleModeSelection
	case 127, CableClub_EventScript_CancelBattleModeSelection

CableClub_EventScript_CancelBattleModeSelection:: @ 81A4036
	setvar VAR_0x8004, 0
	return

CableClub_EventScript_MultiBattleMode:: @ 81A403C
	setvar VAR_0x8004, 5
	return

CableClub_EventScript_DoubleBattleMode:: @ 81A4042
	special HasEnoughMonsForDoubleBattle
	compare VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS
	goto_if_ne CableClub_EventScript_NeedTwoMonsForDoubleBattle
	setvar VAR_0x8004, 2
	return

CableClub_EventScript_NeedTwoMonsForDoubleBattle:: @ 81A4056
	message CableClub_Text_NeedTwoMonsForDoubleBattle
	waitmessage
	waitbuttonpress
	goto CableClub_EventScript_SelectBattleMode

CableClub_EventScript_SingleBattleMode:: @ 81A4062
	setvar VAR_0x8004, 1
	return

CableClub_EventScript_AbortLinkIncorrectNumberOfBattlers:: @ 81A4068
	switch VAR_0x8004
	case 1, CableClub_EventScript_AbortLinkWrongNumberForSingleBattle
	case 2, CableClub_EventScript_AbortLinkWrongNumberForDoubleBattle
	case 5, CableClub_EventScript_AbortLinkNeedFourPlayers
	goto CableClub_EventScript_AbortLinkIncorrectNumberOfParticipants

CableClub_EventScript_AbortLinkNeedFourPlayers:: @ 81A4093
	special CloseLink
	message CableClub_Text_NeedFourPlayers
	waitmessage
	waitbuttonpress
	goto CableClub_EventScript_ConfirmNumberAndRestart

CableClub_EventScript_AbortLinkWrongNumberForDoubleBattle:: @ 81A40A2
	special CloseLink
	message CableClub_Text_CantDoubleBattleWithXPlayers
	waitmessage
	waitbuttonpress
	goto CableClub_EventScript_ConfirmNumberAndRestart

CableClub_EventScript_AbortLinkWrongNumberForSingleBattle:: @ 81A40B1
	special CloseLink
	message CableClub_Text_CantSingleBattleWithXPlayers
	waitmessage
	waitbuttonpress
	goto CableClub_EventScript_ConfirmNumberAndRestart

CableClub_EventScript_ConfirmNumberAndRestart:: @ 81A40C0
	special CloseLink
	message CableClub_Text_PleaseConfirmNumberAndRestart
	waitmessage
	waitbuttonpress
	release
	end

CableClub_EventScript_TradeCenter:: @ 81A40CC
	goto_if_unset FLAG_SYS_POKEDEX_GET, CableClub_EventScript_TradeCenterUnderInspection
	copyvar VAR_0x8007, VAR_LAST_TALKED
	lock
	faceplayer
	msgbox CableClub_Text_WelcomeToTradeCenter, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq CableClub_EventScript_AbortLink
	compare VAR_RESULT, YES
	goto_if_eq CableClub_EventScript_TryEnterTradeCenter

CableClub_EventScript_TryEnterTradeCenter:: @ 81A40FA
	call CableClub_EventScript_CheckPartyTradeRequirements
	compare VAR_RESULT, 0
	goto_if_eq CableClub_EventScript_AbortLink
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq CableClub_EventScript_AbortLink
	message gText_PleaseWaitForLink
	waitmessage
	special TryTradeLinkup
	waitstate
	compare VAR_RESULT, 1
	goto_if_eq CableClub_EventScript_EnterTradeCenter
	compare VAR_RESULT, 2
	goto_if_eq CableClub_EventScript_AbortLinkSomeoneNotReady
	compare VAR_RESULT, 3
	goto_if_eq CableClub_EventScript_AbortLinkDifferentSelections
	compare VAR_RESULT, 4
	goto_if_eq CableClub_EventScript_AbortLinkIncorrectNumberOfParticipants
	compare VAR_RESULT, 5
	goto_if_eq CableClub_EventScript_AbortLink
	compare VAR_RESULT, 6
	goto_if_eq CableClub_EventScript_AbortLinkConnectionError

CableClub_EventScript_EnterTradeCenter:: @ 81A4166
	setvar VAR_0x8004, 3
	copyvar VAR_CABLE_CLUB_STATE, VAR_0x8004
	messageautoscroll CableClub_Text_PleaseEnter
	waitmessage
	setmetatile 8, 3, METATILE_PokemonCenter_Floor_Plain_Alt, 0
	special DrawWholeMapView
	delay 60
	applymovement VAR_LAST_TALKED, Movement_AttendantFaceRight
	waitmovement 0
	closemessage
	applymovement LOCALID_PLAYER, Movement_PlayerApproachLinkRoomRight
	waitmovement 0
	opendoor 8, 1
	waitdooranim
	applymovement LOCALID_PLAYER, Movement_PlayerEnterLinkRoom
	waitmovement 0
	hideobjectat LOCALID_PLAYER, MAP_PETALBURG_CITY
	closedoor 8, 1
	waitdooranim
	release
	goto CableClub_EventScript_CableClubWarp

CableClub_EventScript_CheckPartyTradeRequirements:: @ 81A41BB
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	compare VAR_RESULT, 2
	goto_if_lt CableClub_EventScript_NeedTwoMonsToTrade
	specialvar VAR_RESULT, GetNameOfEnigmaBerryInPlayerParty
	compare VAR_RESULT, 1
	goto_if_eq CableClub_EventScript_CantTradeEnigmaBerry
	setvar VAR_RESULT, 1
	return

CableClub_EventScript_NeedTwoMonsToTrade:: @ 81A41E1
	message CableClub_Text_NeedTwoMonsToTrade
	waitmessage
	waitbuttonpress
	setvar VAR_RESULT, 0
	return

CableClub_EventScript_CantTradeEnigmaBerry:: @ 81A41EE
	message CableClub_Text_CantTradeEnigmaBerry
	waitmessage
	waitbuttonpress
	setvar VAR_RESULT, 0
	return

CableClub_EventScript_RecordCorner:: @ 81A41FB
	goto_if_unset FLAG_VISITED_MAUVILLE_CITY, CableClub_EventScript_RecordCornerUnderPreparation
	copyvar VAR_0x8007, VAR_LAST_TALKED
	lock
	faceplayer
	msgbox CableClub_Text_WelcomeToRecordCorner, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq CableClub_EventScript_AbortLink
	compare VAR_RESULT, YES
	goto_if_eq CableClub_EventScript_TryEnterRecordCorner

CableClub_EventScript_TryEnterRecordCorner:: @ 81A4229
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq CableClub_EventScript_AbortLink
	message gText_PleaseWaitForLink
	waitmessage
	special TryRecordMixLinkup
	waitstate
	special ValidateMixingGameLanguage
	waitstate
	compare VAR_RESULT, 1
	goto_if_eq CableClub_EventScript_EnterRecordCorner
	compare VAR_RESULT, 2
	goto_if_eq CableClub_EventScript_AbortLinkSomeoneNotReady
	compare VAR_RESULT, 3
	goto_if_eq CableClub_EventScript_AbortLinkDifferentSelections
	compare VAR_RESULT, 4
	goto_if_eq CableClub_EventScript_AbortLinkIncorrectNumberOfParticipants
	compare VAR_RESULT, 5
	goto_if_eq CableClub_EventScript_AbortLink
	compare VAR_RESULT, 6
	goto_if_eq CableClub_EventScript_AbortLinkConnectionError
	compare VAR_RESULT, 7
	goto_if_eq CableClub_EventScript_AbortLinkForeignGame

CableClub_EventScript_EnterRecordCorner:: @ 81A4294
	setvar VAR_0x8004, 4
	copyvar VAR_CABLE_CLUB_STATE, VAR_0x8004
	messageautoscroll CableClub_Text_PleaseEnter
	waitmessage
	setmetatile 11, 3, METATILE_PokemonCenter_Floor_Plain_Alt, 0
	special DrawWholeMapView
	delay 60
	applymovement VAR_LAST_TALKED, Movement_AttendantFaceRight
	waitmovement 0
	closemessage
	applymovement LOCALID_PLAYER, Movement_PlayerApproachLinkRoomRight
	waitmovement 0
	opendoor 11, 1
	waitdooranim
	applymovement LOCALID_PLAYER, Movement_PlayerEnterLinkRoom
	waitmovement 0
	hideobjectat LOCALID_PLAYER, MAP_PETALBURG_CITY
	closedoor 11, 1
	waitdooranim
	release
	goto CableClub_EventScript_CableClubWarp

CableClub_EventScript_AbortLinkForeignGame:: @ 81A42E9
	special CloseLink
	message CableClub_Text_CantMixWithDifferentLanguage
	waitmessage
	waitbuttonpress
	release
	end

CableClub_EventScript_AbortLinkConnectionError:: @ 81A42F5
	special CloseLink
	message Text_LinkErrorPleaseReset
	waitmessage
	waitbuttonpress
	release
	end

CableClub_EventScript_AbortLinkSomeoneNotReady:: @ 81A4301
	special CloseLink
	message Text_SomeoneIsNotReadyToLink
	waitmessage
	waitbuttonpress
	release
	end

CableClub_EventScript_AbortLinkDifferentSelections:: @ 81A430D
	special CloseLink
	message Text_PlayersMadeDifferentSelections
	waitmessage
	waitbuttonpress
	release
	end

CableClub_EventScript_AbortLink:: @ 81A4319
	special CloseLink
	message CableClub_Text_PleaseVisitAgain
	waitmessage
	waitbuttonpress
	release
	end

CableClub_EventScript_CableClubWarp:: @ 81A4325
	special SetCableClubWarp
	special DoCableClubWarp
	waitstate
	end

CableClub_EventScript_AbortLinkIncorrectNumberOfParticipants:: @ 81A432D
	special CloseLink
	message CableClub_Text_IncorrectNumberOfParticipants
	waitmessage
	waitbuttonpress
	release
	end

CableClub_EventScript_TradeCenterUnderInspection:: @ 81A4339
	msgbox gText_SorryTradeCenterInspections, MSGBOX_NPC
	end

CableClub_EventScript_ColosseumUndergoingAdjustments:: @ 81A4342
	msgbox gText_SorryColosseumAdjustments, MSGBOX_NPC
	end

CableClub_EventScript_RecordCornerUnderPreparation:: @ 81A434B
	msgbox gText_SorryRecordCornerPreparation, MSGBOX_NPC
	end

Movement_AttendantFaceDown:: @ 81A4354
	face_down
	step_end

Movement_AttendantFaceRight:: @ 81A4356
	face_right
	step_end

Movement_PlayerExitLinkRoom:: @ 81A4358
	walk_down
	walk_down
	step_end

Movement_PlayerApproachLinkRoomRight:: @ 81A435B
	walk_right
	walk_up
	walk_up
	step_end

Movement_PlayerEnterLinkRoom:: @ 81A435F
	walk_up
	step_end

Movement_PlayerFaceAttendantLeft:: @ 81A4361
	face_left
	step_end

EventScript_CableBoxResults:: @ 81A4363
	lockall
	special ShowLinkBattleRecords
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

EventScript_BattleColosseum_2P_PlayerSpot0:: @ 81A436F
	setvar VAR_0x8005, 0
	special ColosseumPlayerSpotTriggered
	waitstate
	end

EventScript_BattleColosseum_2P_PlayerSpot1:: @ 81A4379
	setvar VAR_0x8005, 1
	special ColosseumPlayerSpotTriggered
	waitstate
	end

EventScript_BattleColosseum_4P_PlayerSpot0:: @ 81A4383
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	waitstate
	compare VAR_RESULT, 0
	goto_if_eq EventScript_BattleColosseum_4P_CancelSpotTrigger
	setvar VAR_0x8005, 0
	special ColosseumPlayerSpotTriggered
	waitstate
	end

EventScript_BattleColosseum_4P_PlayerSpot1:: @ 81A439E
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	waitstate
	compare VAR_RESULT, 0
	goto_if_eq EventScript_BattleColosseum_4P_CancelSpotTrigger
	setvar VAR_0x8005, 1
	special ColosseumPlayerSpotTriggered
	waitstate
	end

EventScript_BattleColosseum_4P_PlayerSpot2:: @ 81A43B9
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	waitstate
	compare VAR_RESULT, 0
	goto_if_eq EventScript_BattleColosseum_4P_CancelSpotTrigger
	setvar VAR_0x8005, 2
	special ColosseumPlayerSpotTriggered
	waitstate
	end

EventScript_BattleColosseum_4P_PlayerSpot3:: @ 81A43D4
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	waitstate
	compare VAR_RESULT, 0
	goto_if_eq EventScript_BattleColosseum_4P_CancelSpotTrigger
	setvar VAR_0x8005, 3
	special ColosseumPlayerSpotTriggered
	waitstate
	end

EventScript_BattleColosseum_4P_CancelSpotTrigger:: @ 81A43EF
	end

EventScript_TradeCenter_Chair0:: @ 81A43F0
	setvar VAR_0x8005, 0
	special PlayerEnteredTradeSeat
	waitstate
	end

EventScript_TradeCenter_Chair1:: @ 81A43FA
	setvar VAR_0x8005, 1
	special PlayerEnteredTradeSeat
	waitstate
	end

EventScript_TradeCenter_Chair2:: @ 81A4404
	setvar VAR_0x8005, 2
	special PlayerEnteredTradeSeat
	waitstate
	end

EventScript_TradeCenter_Chair3:: @ 81A440E
	setvar VAR_0x8005, 3
	special PlayerEnteredTradeSeat
	waitstate
	end

EventScript_RecordCenter_Spot0:: @ 81A4418
	setvar VAR_0x8005, 0
	special RecordMixingPlayerSpotTriggered
	waitstate
	compare VAR_TEMP_1, 0
	goto_if_ne RecordCorner_EventScript_ReceivedGiftItem
	end

EventScript_RecordCenter_Spot1:: @ 81A442D
	setvar VAR_0x8005, 1
	special RecordMixingPlayerSpotTriggered
	waitstate
	compare VAR_TEMP_1, 0
	goto_if_ne RecordCorner_EventScript_ReceivedGiftItem
	end

EventScript_RecordCenter_Spot2:: @ 81A4442
	setvar VAR_0x8005, 2
	special RecordMixingPlayerSpotTriggered
	waitstate
	compare VAR_TEMP_1, 0
	goto_if_ne RecordCorner_EventScript_ReceivedGiftItem
	end

EventScript_RecordCenter_Spot3:: @ 81A4457
	setvar VAR_0x8005, 3
	special RecordMixingPlayerSpotTriggered
	waitstate
	compare VAR_TEMP_1, 0
	goto_if_ne RecordCorner_EventScript_ReceivedGiftItem
	end

RecordCorner_EventScript_ReceivedGiftItem:: @ 81A446C
	bufferitemname 1, VAR_TEMP_1
	message RecordCorner_Text_PlayerSentOverOneX
	waitmessage
	waitbuttonpress
	releaseall
	end

CableClub_EventScript_ReadTrainerCard:: @ 81A4479
	message CableClub_Text_GotToLookAtTrainerCard
	waitmessage
	waitbuttonpress
	fadescreen FADE_TO_BLACK
	special Script_ShowLinkTrainerCard
	waitstate
	end

CableClub_EventScript_ReadTrainerCardColored:: @ 81A4487
	message CableClub_Text_GotToLookAtColoredTrainerCard
	waitmessage
	waitbuttonpress
	fadescreen FADE_TO_BLACK
	special Script_ShowLinkTrainerCard
	waitstate
	end

CableClub_EventScript_TooBusyToNotice:: @ 81A4495
	message CableClub_Text_TooBusyToNotice
	waitmessage
	waitbuttonpress
	closemessage
	end

BattleColosseum_2P_EventScript_Attendant:: @ 81A449E
	special Script_FacePlayer
	message BattleColosseum_2P_Text_TakePlaceStartBattle
	waitmessage
	waitbuttonpress
	special Script_ClearHeldMovement
	closemessage
	end

TradeCenter_EventScript_Attendant:: @ 81A44AD
	special Script_FacePlayer
	message TradeCenter_Text_TakeSeatStartTrade
	waitmessage
	waitbuttonpress
	special Script_ClearHeldMovement
	closemessage
	end

RecordCorner_EventScript_Attendant:: @ 81A44BC
	compare VAR_TEMP_0, 0
	goto_if_ne RecordCorner_EventScript_AlreadyMixed
	special Script_FacePlayer
	message RecordCorner_Text_TakeSeatAndWait
	waitmessage
	waitbuttonpress
	special Script_ClearHeldMovement
	closemessage
	end

RecordCorner_EventScript_AlreadyMixed:: @ 81A44D6
	special Script_FacePlayer
	message RecordCorner_Text_ThanksForComing
	waitmessage
	waitbuttonpress
	special Script_ClearHeldMovement
	closemessage
	end

EventScript_ConfirmLeaveCableClubRoom:: @ 81A44E5
	msgbox Text_TerminateLinkConfirmation, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq EventScript_TerminateLink
	erasebox 0, 0, 29, 19
	end

EventScript_TerminateLink:: @ 81A44FE
	messageautoscroll Text_TerminateLinkPleaseWait
	waitmessage
	special ExitLinkRoom
	end

EventScript_DoLinkRoomExit:: @ 81A4508
	special CleanupLinkRoomState
	special ReturnFromLinkRoom
	waitstate
	end
