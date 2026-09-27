BattleTower_BattleRoom_MapScripts:: @ 81608DB
	map_script MAP_SCRIPT_ON_FRAME_TABLE, BattleTower_BattleRoom_OnFrame
	.byte 0

BattleTower_BattleRoom_OnFrame:: @ 81608E1
	map_script_2 VAR_TEMP_0, 0, BattleTower_BattleRoom_EventScript_BeginBattle
	.2byte 0

BattleTower_BattleRoom_EventScript_BeginBattle:: @ 81608EB
	setvar VAR_TEMP_0, 1
	applymovement LOCALID_PLAYER, BattleTower_BattleRoom_Movement_PlayerApproachBattle
	waitmovement 0

BattleTower_BattleRoom_EventScript_ChallengeTrainer:: @ 81608FA
	special ChooseNextBattleTowerTrainer
	addobject 1
	applymovement 1, BattleTower_BattleRoom_Movement_OpponentEnter
	waitmovement 0
	special CopyBattleTowerTrainerGreeting
	msgbox gStringVar4, MSGBOX_DEFAULT
	waitmessage
	closemessage
	special HealPlayerParty
	setvar VAR_0x8004, SPECIAL_BATTLE_TOWER
	setvar VAR_0x8005, 0
	special DoSpecialTrainerBattle
	waitstate
	switch VAR_RESULT
	case 1, BattleTower_BattleRoom_EventScript_WonBattle
	special LoadPlayerParty
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, 5
	special SetBattleTowerProperty
	setvar VAR_0x8004, 14
	special SetBattleTowerProperty
	setvar VAR_0x8004, 3
	special Script_TryGainNewFanFromCounter
	warp MAP_BATTLE_TOWER_LOBBY, 255, 6, 6
	waitstate

BattleTower_BattleRoom_EventScript_WonBattle:: @ 8160961
	setvar VAR_0x8004, 6
	special SetBattleTowerProperty
	switch VAR_RESULT
	case 8, BattleTower_BattleRoom_EventScript_CompleteSet
	applymovement 1, BattleTower_BattleRoom_Movement_OpponentExit
	waitmovement 0
	removeobject 1
	applymovement 2, BattleTower_BattleRoom_Movement_AttendantApproachPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, BattleTower_BattleRoom_Movement_PlayerFaceDown
	waitmovement 0
	msgbox BattleTower_BattleRoom_Text_RestoreYourPokemon, MSGBOX_DEFAULT
	special LoadPlayerParty
	special SavePlayerParty
	special SetBattleTowerParty
	playfanfare MUS_HEAL
	waitfanfare
	special HealPlayerParty

BattleTower_BattleRoom_EventScript_AskContinue:: @ 81609B2
	msgbox BattleTower_BattleRoom_Text_ReadyForNextOpponent, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, BattleTower_BattleRoom_EventScript_AskSaveAndQuit
	case YES, BattleTower_BattleRoom_EventScript_ContinueChallenge
	case 127, BattleTower_BattleRoom_EventScript_AskSaveAndQuit

BattleTower_BattleRoom_EventScript_AskSaveAndQuit:: @ 81609E0
	msgbox BattleTower_BattleRoom_Text_SaveAndQuitQuestion, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, BattleTower_BattleRoom_EventScript_AskCancelChallenge
	case YES, BattleTower_BattleRoom_EventScript_SaveAndQuit
	case 127, BattleTower_BattleRoom_EventScript_AskCancelChallenge

BattleTower_BattleRoom_EventScript_AskCancelChallenge:: @ 8160A0E
	msgbox BattleTower_BattleRoom_Text_CancelChallengeQuestion, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, BattleTower_BattleRoom_EventScript_AskContinue
	case YES, BattleTower_BattleRoom_EventScript_SoftReset
	case 127, BattleTower_BattleRoom_EventScript_AskContinue

BattleTower_BattleRoom_EventScript_ContinueChallenge:: @ 8160A3C
	closemessage
	applymovement LOCALID_PLAYER, BattleTower_BattleRoom_Movement_PlayerFaceOpponent
	waitmovement 0
	applymovement 2, BattleTower_BattleRoom_Movement_AttendantReturn
	waitmovement 0
	goto BattleTower_BattleRoom_EventScript_ChallengeTrainer

BattleTower_BattleRoom_EventScript_CompleteSet:: @ 8160A56
	special LoadPlayerParty
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, 4
	special SetBattleTowerProperty
	setvar VAR_0x8004, 2
	setvar VAR_0x8005, 1
	special SetBattleTowerProperty
	setvar VAR_0x8004, 7
	special SetBattleTowerProperty
	setvar VAR_0x8004, 14
	special SetBattleTowerProperty
	setvar VAR_0x8004, 3
	special Script_TryGainNewFanFromCounter
	warp MAP_BATTLE_TOWER_LOBBY, 255, 6, 6
	waitstate

BattleTower_BattleRoom_EventScript_SaveAndQuit:: @ 8160A94
	setvar VAR_0x8004, 2
	special SaveBattleTowerProgress
	message gText_SavingDontTurnOff
	delay 60
	waitmessage
	playse SE_SAVE
	waitse
	msgbox gText_PlayerSavedGame, MSGBOX_DEFAULT

BattleTower_BattleRoom_EventScript_SoftReset:: @ 8160AB1
	fadescreen FADE_TO_BLACK
	special BattleTowerSoftReset
	end

BattleTower_BattleRoom_Movement_PlayerApproachBattle:: @ 8160AB7
	walk_up
	walk_up
	walk_up
	face_right
	step_end

BattleTower_BattleRoom_Movement_PlayerFaceDown:: @ 8160ABC
	face_down
	step_end

BattleTower_BattleRoom_Movement_PlayerFaceOpponent:: @ 8160ABE
	face_right
	step_end

BattleTower_BattleRoom_Movement_OpponentEnter:: @ 8160AC0
	walk_down
	walk_down
	walk_down
	walk_down
	face_left
	step_end

BattleTower_BattleRoom_Movement_OpponentExit:: @ 8160AC6
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

BattleTower_BattleRoom_Movement_AttendantApproachPlayer:: @ 8160ACB
	walk_right
	walk_right
	walk_right
	walk_up
	step_end

BattleTower_BattleRoom_Movement_AttendantReturn:: @ 8160AD0
	walk_down
	walk_left
	walk_left
	walk_left
	face_right
	step_end
