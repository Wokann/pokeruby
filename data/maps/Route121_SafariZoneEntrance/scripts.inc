Route121_SafariZoneEntrance_MapScripts:: @ 815C323
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route121_SafariZoneEntrance_OnFrame
	.byte 0

Route121_SafariZoneEntrance_OnFrame:: @ 815C329
	map_script_2 VAR_SAFARI_ZONE_STATE, 1, Route121_SafariZoneEntrance_EventScript_ExitSafariZone
	.2byte 0

Route121_SafariZoneEntrance_EventScript_ExitSafariZone:: @ 815C333
	lockall
	applymovement LOCALID_PLAYER, Route121_SafariZoneEntrance_Movement_ExitSafariZone
	waitmovement 0
	special ExitSafariMode
	setvar VAR_SAFARI_ZONE_STATE, 0
	releaseall
	end

Route121_SafariZoneEntrance_Movement_ExitSafariZone:: @ 815C348
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route121_SafariZoneEntrance_EventScript_WelcomeAttendant:: @ 815C351
	msgbox Route121_SafariZoneEntrance_Text_WelcomeToSafariZone, MSGBOX_NPC
	end

Route121_SafariZoneEntrance_EventScript_InfoAttendant:: @ 815C35A
	lock
	faceplayer
	msgbox Route121_SafariZoneEntrance_Text_WelcomeFirstTime, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route121_SafariZoneEntrance_EventScript_FirstTimeInfo
	msgbox Route121_SafariZoneEntrance_Text_ComeInAndEnjoy, MSGBOX_DEFAULT
	release
	end

Route121_SafariZoneEntrance_EventScript_FirstTimeInfo:: @ 815C379
	msgbox Route121_SafariZoneEntrance_Text_FirstTimeInfo, MSGBOX_DEFAULT
	release
	end

Route121_SafariZoneEntrance_EventScript_EntranceCounterTrigger:: @ 815C383
	lockall
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	showmoneybox 0, 0
	nop
	msgbox Route121_SafariZoneEntrance_Text_WouldYouLikeToPlay, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route121_SafariZoneEntrance_EventScript_TryEnterSafariZone
	msgbox Route121_SafariZoneEntrance_Text_PlayAnotherTime, MSGBOX_DEFAULT
	goto Route121_SafariZoneEntrance_EventScript_MovePlayerBackFromCounter
	end

Route121_SafariZoneEntrance_EventScript_TryEnterSafariZone:: @ 815C3B3
	checkitem ITEM_POKEBLOCK_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq Route121_SafariZoneEntrance_EventScript_NoPokeblockCase
	call Route121_SafariZoneEntrance_EventScript_CheckHasRoomForPokemon
	checkmoney 500, 0
	compare VAR_RESULT, 0
	goto_if_eq Route121_SafariZoneEntrance_EventScript_NotEnoughMoney
	msgbox Route121_SafariZoneEntrance_Text_ThatWillBe500Please, MSGBOX_DEFAULT
	removemoney 500, 0
	updatemoneybox 0, 0
	nop
	msgbox Route121_SafariZoneEntrance_Text_HereAreYourSafariBalls, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message Route121_SafariZoneEntrance_Text_Received30SafariBalls
	waitfanfare
	waitmessage
	msgbox Route121_SafariZoneEntrance_Text_PleaseEnjoyYourself, MSGBOX_DEFAULT
	closemessage
	hidemoneybox 0, 0
	applymovement LOCALID_PLAYER, Route121_SafariZoneEntrance_Movement_EnterSafariZone
	waitmovement 0
	special EnterSafariMode
	setvar VAR_SAFARI_ZONE_STATE, 2
	warp MAP_SAFARI_ZONE_SOUTHEAST, 255, 32, 33
	waitstate
	end

Route121_SafariZoneEntrance_EventScript_CheckHasRoomForPokemon:: @ 815C425
	getpartysize
	compare VAR_RESULT, 6
	goto_if_ne Route121_SafariZoneEntrance_EventScript_HasRoomForPokemon
	specialvar VAR_RESULT, CheckFreePokemonStorageSpace
	compare VAR_RESULT, 1
	goto_if_eq Route121_SafariZoneEntrance_EventScript_HasRoomForPokemon
	msgbox Route121_SafariZoneEntrance_Text_PCIsFull, MSGBOX_DEFAULT
	goto Route121_SafariZoneEntrance_EventScript_MovePlayerBackFromCounter
	end

Route121_SafariZoneEntrance_EventScript_HasRoomForPokemon:: @ 815C44F
	return

Route121_SafariZoneEntrance_EventScript_NoPokeblockCase:: @ 815C450
	msgbox Route121_SafariZoneEntrance_Text_YouNeedPokeblockCase, MSGBOX_DEFAULT
	goto Route121_SafariZoneEntrance_EventScript_MovePlayerBackFromCounter
	end

Route121_SafariZoneEntrance_EventScript_NotEnoughMoney:: @ 815C45E
	msgbox Route121_SafariZoneEntrance_Text_NotEnoughMoney, MSGBOX_DEFAULT
	goto Route121_SafariZoneEntrance_EventScript_MovePlayerBackFromCounter
	end

Route121_SafariZoneEntrance_EventScript_MovePlayerBackFromCounter:: @ 815C46C
	closemessage
	hidemoneybox 0, 0
	applymovement LOCALID_PLAYER, Route121_SafariZoneEntrance_Movement_BackAwayFromCounter
	waitmovement 0
	releaseall
	end

Route121_SafariZoneEntrance_Movement_BackAwayFromCounter:: @ 815C47C
	walk_right
	step_end

Route121_SafariZoneEntrance_Movement_EnterSafariZone:: @ 815C47E
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	delay_16
	step_end

Route121_SafariZoneEntrance_EventScript_TrainerTipSign:: @ 815C487
	msgbox Route121_SafariZoneEntrance_Text_TrainerTip, MSGBOX_SIGN
	end
