GraniteCave_StevensRoom_MapScripts:: @ 815CBF9
	.byte 0

GraniteCave_StevensRoom_EventScript_Steven:: @ 815CBFA
	lock
	faceplayer
	msgbox GraniteCave_StevensRoom_Text_ImStevenLetterForMe, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_LETTER
	call Common_EventScript_PlayerHandedOverTheItem
	setflag FLAG_DELIVERED_STEVEN_LETTER
	msgbox GraniteCave_StevensRoom_Text_ThankYouTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_TM47_STEEL_WING
	compare VAR_RESULT, 0
	call_if_eq GraniteCave_StevensRoom_EventScript_BagFull
	msgbox GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister, MSGBOX_DEFAULT
	closemessage
	compare VAR_FACING, 2
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitNorthWestEast
	compare VAR_FACING, 1
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitSouth
	compare VAR_FACING, 3
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitNorthWestEast
	compare VAR_FACING, 4
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitNorthWestEast
	playse SE_EXIT
	removeobject 1
	release
	end

GraniteCave_StevensRoom_EventScript_StevenExitNorthWestEast:: @ 815CC6D
	applymovement 1, GraniteCave_StevensRoom_Movement_StevenExitNorthWestEast
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_StevenExitSouth:: @ 815CC78
	applymovement 1, GraniteCave_StevensRoom_Movement_StevenExitSouth
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_BagFull:: @ 815CC83
	msgbox GraniteCave_StevensRoom_Text_OhBagIsFull, MSGBOX_DEFAULT
	return

GraniteCave_StevensRoom_Movement_StevenExitNorthWestEast:: @ 815CC8C
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	delay_8
	step_end

GraniteCave_StevensRoom_Movement_StevenExitSouth:: @ 815CC93
	walk_left
	walk_up
	walk_up
	walk_up
	walk_right
	walk_up
	walk_up
	delay_8
	step_end
