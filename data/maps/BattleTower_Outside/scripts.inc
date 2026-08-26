BattleTower_Outside_MapScripts:: @ 8160156
	map_script MAP_SCRIPT_ON_TRANSITION, BattleTower_Outside_OnTransition
	.byte 0

BattleTower_Outside_OnTransition:: @ 816015C
	setflag FLAG_LANDMARK_BATTLE_TOWER
	setvar VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 0
	setflag FLAG_HIDE_REPORTER_BATTLE_TOWER
	end

BattleTower_Outside_EventScript_FerryAttendant:: @ 8160168
	lock
	faceplayer
	msgbox BattleTower_Outside_Text_CanISeeYourTicket, MSGBOX_DEFAULT
	checkitem ITEM_SS_TICKET, 1
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Outside_EventScript_NoTicket
	message BattleTower_Outside_Text_TicketAccepted
	waitmessage
	goto BattleTower_Outside_EventScript_AskFerryDestination
	end

BattleTower_Outside_EventScript_AskFerryDestination:: @ 816018E
	multichoicedefault 21, 6, 53, 2, 0
	switch VAR_RESULT
	case 0, BattleTower_Outside_EventScript_SailToSlateport
	case 1, BattleTower_Outside_EventScript_SailToLilycove
	case 2, BattleTower_Outside_EventScript_DeclineFerry
	case 127, BattleTower_Outside_EventScript_DeclineFerry
	end

BattleTower_Outside_EventScript_NoTicket:: @ 81601C6
	msgbox BattleTower_Outside_Text_NoTicket, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_SailToSlateport:: @ 81601D0
	msgbox BattleTower_Outside_Text_SlateportItIs, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Outside_EventScript_ChooseAnotherDestination
	msgbox BattleTower_Outside_Text_BoardFerry, MSGBOX_DEFAULT
	call BattleTower_Outside_EventScript_BoardFerry
	warp MAP_SLATEPORT_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BattleTower_Outside_EventScript_SailToLilycove:: @ 81601FB
	msgbox BattleTower_Outside_Text_LilycoveItIs, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Outside_EventScript_ChooseAnotherDestination
	msgbox BattleTower_Outside_Text_BoardFerry, MSGBOX_DEFAULT
	call BattleTower_Outside_EventScript_BoardFerry
	warp MAP_LILYCOVE_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BattleTower_Outside_EventScript_ChooseAnotherDestination:: @ 8160226
	message BattleTower_Outside_Text_WhereWouldYouLikeToGo
	waitmessage
	goto BattleTower_Outside_EventScript_AskFerryDestination
	end

BattleTower_Outside_EventScript_BoardFerry:: @ 8160232
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 30
	hideobjectat 5, MAP_BATTLE_TOWER_OUTSIDE
	setvar VAR_0x8004, 4
	call Common_EventScript_FerryDepartIsland
	return

BattleTower_Outside_EventScript_DeclineFerry:: @ 8160250
	msgbox BattleTower_Outside_Text_SailWithUsAnotherTime, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_BattleTowerSign:: @ 816025A
	msgbox BattleTower_Outside_Text_BattleTowerSign, MSGBOX_SIGN
	end

BattleTower_Outside_EventScript_BugCatcher:: @ 8160263
	msgbox BattleTower_Outside_Text_BugCatcher, MSGBOX_NPC
	end

BattleTower_Outside_EventScript_Woman:: @ 816026C
	msgbox BattleTower_Outside_Text_Woman, MSGBOX_NPC
	end

BattleTower_Outside_EventScript_Boy:: @ 8160275
	msgbox BattleTower_Outside_Text_Boy, MSGBOX_NPC
	end
