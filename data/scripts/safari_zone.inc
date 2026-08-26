SafariZone_EventScript_OutOfBallsMidBattle:: @ 81C340A
	setvar VAR_SAFARI_ZONE_STATE, 1
	special ExitSafariMode
	setwarp MAP_ROUTE121_SAFARI_ZONE_ENTRANCE, 255, 2, 5
	end

SafariZone_EventScript_Exit: @ 81C341B
	setvar VAR_SAFARI_ZONE_STATE, 1
	special ExitSafariMode
	warp MAP_ROUTE121_SAFARI_ZONE_ENTRANCE, 255, 2, 5
	waitstate
	end

SafariZone_EventScript_RetirePrompt:: @ 81C342D
	lockall
	msgbox SafariZone_Text_WouldYouLikeToExit, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SafariZone_EventScript_Retire
	releaseall
	end

SafariZone_EventScript_Retire:
	goto SafariZone_EventScript_Exit

SafariZone_EventScript_TimesUp:: @ 81C3448
	lockall
	playse SE_DING_DONG
	message SafariZone_Text_TimesUp
	waitmessage
	waitbuttonpress
	releaseall
	goto SafariZone_EventScript_Exit

SafariZone_EventScript_OutOfBalls:: @ 81C3459
	lockall
	playse SE_DING_DONG
	message SafariZone_Text_OutOfBalls
	waitmessage
	waitbuttonpress
	releaseall
	goto SafariZone_EventScript_Exit

EventScript_PokeBlockFeeder:: @ 81C346A
	lockall
	special SafariZoneGetPokeblockNameInFeeder
	compare VAR_RESULT, 0xFFFF
	goto_if_ne SafariZone_EventScript_PokeblockPresent
	msgbox SafariZone_Text_PlacePokeblockOnFeeder, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SafariZone_EventScript_ChoosePokeblock
	releaseall
	end

SafariZone_EventScript_ChoosePokeblock:
	fadescreen FADE_TO_BLACK
	special OpenPokeblockCaseOnFeeder
	waitstate
	compare VAR_RESULT, 0xFFFF
	goto_if_ne SafariZone_EventScript_PokeblockPlaced
	end

SafariZone_EventScript_PokeblockPlaced:
	message SafariZone_Text_PokeblockWasPlaced
	waitmessage
	waitbuttonpress
	releaseall
	end

SafariZone_EventScript_PokeblockPresent:
	message SafariZone_Text_PokeblockStillHere
	waitmessage
	waitbuttonpress
	releaseall
	end
