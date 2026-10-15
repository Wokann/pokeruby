BattleTower_Outside_MapScripts:: @ 8160156
	map_script MAP_SCRIPT_ON_TRANSITION, BattleTower_Outside_OnTransition
	.byte 0

BattleTower_Outside_OnTransition:: @ 816015C
	setflag FLAG_LANDMARK_BATTLE_TOWER
	setvar VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 0
	setflag FLAG_HIDE_BATTLE_TOWER_REPORTER
	end

BattleTower_Outside_EventScript_FerryAttendant:: @ 8160168
	lock
	faceplayer
	msgbox BattleTower_Outside_Text_MayISeeYourTicket, MSGBOX_DEFAULT
	checkitem ITEM_SS_TICKET, 1
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Outside_EventScript_NoSSTicket
	message BattleTower_Outside_Text_WhereWouldYouLikeToGo
	waitmessage
	goto BattleTower_Outside_EventScript_ChooseFerryDestination
	end

BattleTower_Outside_EventScript_ChooseFerryDestination:: @ 816018E
	multichoicedefault 21, 6, 53, 2, 0
	switch VAR_RESULT
	case 0, BattleTower_Outside_EventScript_FerryToSlateport
	case 1, BattleTower_Outside_EventScript_FerryToLilycove
	case 2, BattleTower_Outside_EventScript_CancelFerrySelect
	case 127, BattleTower_Outside_EventScript_CancelFerrySelect
	end

BattleTower_Outside_EventScript_NoSSTicket:: @ 81601C6
	msgbox BattleTower_Outside_Text_MustHaveTicketToBoard, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_FerryToSlateport:: @ 81601D0
	msgbox BattleTower_Outside_Text_SlateportItIs, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Outside_EventScript_ChooseNewFerryDestination
	msgbox BattleTower_Outside_Text_PleaseBoardFerry, MSGBOX_DEFAULT
	call BattleTower_Outside_EventScript_BoardFerry
	warp MAP_SLATEPORT_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BattleTower_Outside_EventScript_FerryToLilycove:: @ 81601FB
	msgbox BattleTower_Outside_Text_LilycoveItIs, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Outside_EventScript_ChooseNewFerryDestination
	msgbox BattleTower_Outside_Text_PleaseBoardFerry, MSGBOX_DEFAULT
	call BattleTower_Outside_EventScript_BoardFerry
	warp MAP_LILYCOVE_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BattleTower_Outside_EventScript_ChooseNewFerryDestination:: @ 8160226
	message BattleTower_Outside_Text_ThenWhereWouldYouLikeToGo
	waitmessage
	goto BattleTower_Outside_EventScript_ChooseFerryDestination
	end

BattleTower_Outside_EventScript_BoardFerry:: @ 8160232
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 30
	hideobjectat 5, MAP_BATTLE_TOWER_OUTSIDE
	setvar VAR_0x8004, 4
	call Common_EventScript_FerryDepartIsland
	return

BattleTower_Outside_EventScript_CancelFerrySelect:: @ 8160250
	msgbox BattleTower_Outside_Text_SailWithUsAnotherTime, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_16025A:: @ 816025A
	msgbox BattleTower_Outside_Text_199C6F, MSGBOX_SIGN
	end

BattleTower_Outside_EventScript_160263:: @ 8160263
	msgbox BattleTower_Outside_Text_ThisIsBattleTower, MSGBOX_NPC
	end

BattleTower_Outside_EventScript_16026C:: @ 816026C
	msgbox BattleTower_Outside_Text_199E9C, MSGBOX_NPC
	end

BattleTower_Outside_EventScript_160275:: @ 8160275
	msgbox BattleTower_Outside_Text_199ED4, MSGBOX_NPC
	end
