MossdeepCity_GameCorner_1F_MapScripts:: @ 815AC52
	map_script MAP_SCRIPT_ON_LOAD, MossdeepCity_GameCorner_1F_MapScript1_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, MossdeepCity_GameCorner_1F_MapScript1_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MossdeepCity_GameCorner_1F_OnFrame
	.byte 0

MossdeepCity_GameCorner_1F_MapScript1_OnLoad:: @ 815AC62
	special ValidateEReaderTrainer
	compare VAR_RESULT, 1
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_15AC71
	end

MossdeepCity_GameCorner_1F_EventScript_15AC71:: @ 815AC71
	setmetatile 3, 0, METATILE_GenericBuilding_Doorway_ClosedTop, 1
	setmetatile 3, 1, METATILE_GenericBuilding_Doorway_ClosedBottom, 1
	return

MossdeepCity_GameCorner_1F_MapScript1_OnTransition:: @ 815AC84
	special ValidateEReaderTrainer
	compare VAR_RESULT, 0
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_15AC9E
	compare VAR_GAME_CORNER_STATE, 0
	call_if_ne MossdeepCity_GameCorner_1F_EventScript_MoveOldManToDoor
	end

MossdeepCity_GameCorner_1F_EventScript_15AC9E:: @ 815AC9E
	setvar VAR_TEMP_1, 1
	setobjectxyperm 1, 3, 2
	setobjectmovementtype 1, 8
	return

MossdeepCity_GameCorner_1F_EventScript_MoveOldManToDoor:: @ 815ACAF
	setobjectxyperm 1, 2, 2
	setobjectmovementtype 1, 10
	return

MossdeepCity_GameCorner_1F_OnFrame:: @ 815ACBB
	map_script_2 VAR_GAME_CORNER_STATE, 1, MossdeepCity_GameCorner_1F_EventScript_OldManCommentOnBattle
	map_script_2 VAR_GAME_CORNER_STATE, 2, MossdeepCity_GameCorner_1F_EventScript_OldManCommentOnBattle
	map_script_2 VAR_GAME_CORNER_STATE, 3, MossdeepCity_GameCorner_1F_EventScript_OldManCommentOnBattle
	.2byte 0

MossdeepCity_GameCorner_1F_EventScript_OldManCommentOnBattle:: @ 815ACD5
	lockall
	applymovement LOCALID_PLAYER, MossdeepCity_GameCorner_1F_Movement_PlayerExitStairs
	waitmovement 0
	applymovement 1, MossdeepCity_GameCorner_1F_Movement_OldManWalkBehindPlayer
	waitmovement 0
	copyobjectxytoperm 1
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	compare VAR_GAME_CORNER_STATE, 1
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_BattleWonComment
	compare VAR_GAME_CORNER_STATE, 2
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_BattleLostComment
	compare VAR_GAME_CORNER_STATE, 3
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_BattleTiedComment
	special LoadPlayerParty
	setvar VAR_GAME_CORNER_STATE, 0
	releaseall
	end

MossdeepCity_GameCorner_1F_EventScript_BattleWonComment:: @ 815AD22
	msgbox RS_MysteryEventsHouse_Text_BattleWon, MSGBOX_DEFAULT
	return

MossdeepCity_GameCorner_1F_EventScript_BattleLostComment:: @ 815AD2B
	msgbox RS_MysteryEventsHouse_Text_BattleLost, MSGBOX_DEFAULT
	return

MossdeepCity_GameCorner_1F_EventScript_BattleTiedComment:: @ 815AD34
	msgbox RS_MysteryEventsHouse_Text_BattleTie, MSGBOX_DEFAULT
	return

MossdeepCity_GameCorner_1F_Movement_PlayerExitStairs:: @ 815AD3D
	walk_down
	step_end

MossdeepCity_GameCorner_1F_Movement_OldManWalkBehindPlayer:: @ 815AD3F
	walk_right
	walk_in_place_faster_down
	step_end

MossdeepCity_GameCorner_1F_EventScript_15AD42:: @ 815AD42
	lock
	faceplayer
	compare VAR_TEMP_1, 1
	goto_if_eq MossdeepCity_GameCorner_1F_EventScript_15AD59
	msgbox RS_MysteryEventsHouse_Text_OldManGreeting, MSGBOX_DEFAULT
	release
	end

MossdeepCity_GameCorner_1F_EventScript_15AD59:: @ 815AD59
	special SavePlayerParty
	special BufferEReaderTrainerName
	msgbox RS_MysteryEventsHouse_Text_ChallengeVisitingTrainer, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MossdeepCity_GameCorner_1F_EventScript_DeclineBattle
	call MossdeepCity_GameCorner_1F_EventScript_15ADF5
	compare VAR_RESULT, 0
	goto_if_eq MossdeepCity_GameCorner_1F_EventScript_DeclineBattle
	msgbox RS_MysteryEventsHouse_Text_SaveYourProgress, MSGBOX_YESNO
	compare VAR_RESULT, NO
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_DeclineBattle
	special LoadPlayerParty
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq MossdeepCity_GameCorner_1F_EventScript_DeclineBattle
	erasebox 0, 0, 15, 10
	special SavePlayerParty
	special ReducePlayerPartyToSelectedMons
	msgbox RS_MysteryEventsHouse_Text_HopeToSeeAGoodMatch, MSGBOX_DEFAULT
	closemessage
	compare VAR_FACING, 2
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_EnterBasementNorth
	compare VAR_FACING, 4
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_EnterBasementEast
	compare VAR_FACING, 3
	call_if_eq MossdeepCity_GameCorner_1F_EventScript_EnterBasementWest
	warp MAP_MOSSDEEP_CITY_GAME_CORNER_B1F, 255, 3, 1
	waitstate
	release
	end

MossdeepCity_GameCorner_1F_EventScript_DeclineBattle:: @ 815ADE8
	special LoadPlayerParty
	msgbox RS_MysteryEventsHouse_Text_YouWontBattle, MSGBOX_DEFAULT
	release
	end

MossdeepCity_GameCorner_1F_EventScript_15ADF5:: @ 815ADF5
	msgbox RS_MysteryEventsHouse_Text_KeepItToA3On3, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	waitstate
	return

MossdeepCity_GameCorner_1F_EventScript_EnterBasementNorth:: @ 815AE04
	applymovement VAR_LAST_TALKED, MossdeepCity_GameCorner_1F_Movement_OldManMoveAsideLeft
	applymovement LOCALID_PLAYER, MossdeepCity_GameCorner_1F_Movement_PlayerEnterBasementNorth
	waitmovement 0
	return

MossdeepCity_GameCorner_1F_EventScript_EnterBasementEast:: @ 815AE16
	applymovement VAR_LAST_TALKED, MossdeepCity_GameCorner_1F_Movement_OldManMoveAsideRight
	applymovement LOCALID_PLAYER, MossdeepCity_GameCorner_1F_Movement_PlayerEnterBasementEast
	waitmovement 0
	return

MossdeepCity_GameCorner_1F_EventScript_EnterBasementWest:: @ 815AE28
	applymovement VAR_LAST_TALKED, MossdeepCity_GameCorner_1F_Movement_OldManMoveAsideLeft
	applymovement LOCALID_PLAYER, MossdeepCity_GameCorner_1F_Movement_PlayerEnterBasementWest
	waitmovement 0
	return

MossdeepCity_GameCorner_1F_Movement_PlayerEnterBasementNorth:: @ 815AE3A
	delay_16
	walk_up
	walk_up
	step_end

MossdeepCity_GameCorner_1F_Movement_PlayerEnterBasementEast:: @ 815AE3E
	delay_16
	walk_right
	walk_up
	step_end

MossdeepCity_GameCorner_1F_Movement_PlayerEnterBasementWest:: @ 815AE42
	delay_16
	walk_left
	walk_up
	step_end

MossdeepCity_GameCorner_1F_Movement_OldManMoveAsideLeft:: @ 815AE46
	walk_left
	walk_in_place_faster_right
	step_end

MossdeepCity_GameCorner_1F_Movement_OldManMoveAsideRight:: @ 815AE49
	walk_right
	walk_in_place_faster_left
	step_end

RS_MysteryEventsHouse_EventScript_Door:: @ 815AE4C
	msgbox RS_MysteryEventsHouse_Text_DoorIsLocked, MSGBOX_SIGN
	end
