MossdeepCity_GameCorner_B1F_MapScripts:: @ 815AE55
	map_script MAP_SCRIPT_ON_TRANSITION, MossdeepCity_MysteryEventsHouse_BattleRoom_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MossdeepCity_MysteryEventsHouse_BattleRoom_OnFrame
	.byte 0

MossdeepCity_MysteryEventsHouse_BattleRoom_OnTransition:: @ 815AE60
	special SetEReaderTrainerGfxId
	end

MossdeepCity_MysteryEventsHouse_BattleRoom_OnFrame:: @ 815AE64
	map_script_2 VAR_TEMP_1, 0, MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_StartEReaderTrainerBattle
	.2byte 0

MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_StartEReaderTrainerBattle:: @ 815AE6E
	lockall
	applymovement LOCALID_PLAYER, MossdeepCity_MysteryEventsHouse_BattleRoom_Movement_PlayerEnter
	waitmovement 0
	special PrintEReaderTrainerGreeting
	msgbox gStringVar4, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 2
	setvar VAR_0x8005, 0
	special StartSpecialBattle
	waitstate
	compare VAR_RESULT, 3
	call_if_eq MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_HandleBattleTie
	compare VAR_RESULT, 1
	call_if_eq MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_HandleBattleWon
	compare VAR_RESULT, 2
	call_if_eq MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_HandleBattleLost
	closemessage
	special ScrSpecial_HealPlayerParty
	applymovement LOCALID_PLAYER, MossdeepCity_MysteryEventsHouse_BattleRoom_Movement_PlayerExit
	waitmovement 0
	special LoadPlayerParty
	setvar VAR_TEMP_1, 1
	warp MAP_MOSSDEEP_CITY_GAME_CORNER_1F, 255, 3, 1
	waitstate
	releaseall
	end

MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_HandleBattleTie:: @ 815AED5
	setvar VAR_E_READER_TRAINER_BATTLE_RESULT, 3
	msgbox MossdeepCity_MysteryEventsHouse_Text_MatchEndedUpDraw, MSGBOX_DEFAULT
	return

MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_HandleBattleWon:: @ 815AEE3
	setvar VAR_E_READER_TRAINER_BATTLE_RESULT, 1
	special ShowFieldMessageStringVar4
	waitmessage
	waitbuttonpress
	return

MossdeepCity_MysteryEventsHouse_BattleRoom_EventScript_HandleBattleLost:: @ 815AEEE
	setvar VAR_E_READER_TRAINER_BATTLE_RESULT, 2
	special ShowFieldMessageStringVar4
	waitmessage
	waitbuttonpress
	return

MossdeepCity_MysteryEventsHouse_BattleRoom_Movement_PlayerEnter:: @ 815AEF9
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	step_end

MossdeepCity_MysteryEventsHouse_BattleRoom_Movement_PlayerExit:: @ 815AEFF
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	delay_8
	step_end
