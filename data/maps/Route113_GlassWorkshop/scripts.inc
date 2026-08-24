Route113_GlassWorkshop_MapScripts:: @ 816354A
	map_script MAP_SCRIPT_ON_TRANSITION, Route113_GlassWorkshop_OnTransition
	.byte 0

	.set BLUE_FLUTE_PRICE,   250
	.set YELLOW_FLUTE_PRICE, 500
	.set RED_FLUTE_PRICE,    500
	.set WHITE_FLUTE_PRICE,  1000
	.set BLACK_FLUTE_PRICE,  1000
	.set PRETTY_CHAIR_PRICE, 6000
	.set PRETTY_DESK_PRICE,  8000
	.set LOWEST_ASH_PRICE,   BLUE_FLUTE_PRICE

Route113_GlassWorkshop_OnTransition:: @ 8163550
	setflag FLAG_LANDMARK_GLASS_WORKSHOP
	compare VAR_GLASS_WORKSHOP_STATE, 1
	call_if_eq Route113_GlassWorkshop_EventScript_ReenterWorkshopAfterSootSack
	end

Route113_GlassWorkshop_EventScript_ReenterWorkshopAfterSootSack:: @ 816355F
	setvar VAR_GLASS_WORKSHOP_STATE, 2
	return

Route113_GlassWorkshop_EventScript_GlassWorker:: @ 8163565
	lock
	faceplayer
	compare VAR_GLASS_WORKSHOP_STATE, 10
	goto_if_ge Route113_GlassWorkshop_EventScript_GiveItemAfterNoRoom
	compare VAR_GLASS_WORKSHOP_STATE, 2
	goto_if_eq Route113_GlassWorkshop_EventScript_CheckCollectedAsh
	compare VAR_GLASS_WORKSHOP_STATE, 1
	goto_if_eq Route113_GlassWorkshop_EventScript_ExplainSootSack
	msgbox Route113_GlassWorkshop_Text_GoCollectAshWithThis, MSGBOX_DEFAULT
	giveitem ITEM_SOOT_SACK
	setvar VAR_GLASS_WORKSHOP_STATE, 1
	msgbox Route113_GlassWorkshop_Text_ExplainSootSack, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_ExplainSootSack:: @ 81635AB
	msgbox Route113_GlassWorkshop_Text_ExplainSootSack, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_CheckCollectedAsh:: @ 81635B5
	checkitem ITEM_SOOT_SACK, 1
	compare VAR_RESULT, 0
	goto_if_eq Route113_GlassWorkshop_EventScript_SootSackNotInBag
	msgbox Route113_GlassWorkshop_Text_LetsSeeCollectedAshes, MSGBOX_DEFAULT
	compare VAR_ASH_GATHER_COUNT, LOWEST_ASH_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAsh
	message Route113_GlassWorkshop_Text_WhichGlassItemWoudYouLike
	waitmessage
	goto Route113_GlassWorkshop_EventScript_ChooseGlassItem
	end

Route113_GlassWorkshop_EventScript_SootSackNotInBag:: @ 81635E4
	msgbox Route113_GlassWorkshop_Text_HaventGotYourSootSack, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_ChooseGlassItem:: @ 81635EE
	setvar VAR_0x8009, 0
	special ShowGlassWorkshopMenu
	waitstate
	switch VAR_RESULT
	case 0, Route113_GlassWorkshop_EventScript_BlueFlute
	case 1, Route113_GlassWorkshop_EventScript_YellowFlute
	case 2, Route113_GlassWorkshop_EventScript_RedFlute
	case 3, Route113_GlassWorkshop_EventScript_WhiteFlute
	case 4, Route113_GlassWorkshop_EventScript_BlackFlute
	case 5, Route113_GlassWorkshop_EventScript_PrettyChair
	case 6, Route113_GlassWorkshop_EventScript_PrettyDesk
	case 7, Route113_GlassWorkshop_EventScript_CancelGlassItemSelect
	case 127, Route113_GlassWorkshop_EventScript_CancelGlassItemSelect
	end

Route113_GlassWorkshop_EventScript_BlueFlute:: @ 8163660
	setvar VAR_0x8008, ITEM_BLUE_FLUTE
	bufferitemname 0, VAR_0x8008
	setvar VAR_0x800A, BLUE_FLUTE_PRICE
	compare VAR_ASH_GATHER_COUNT, BLUE_FLUTE_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	setvar VAR_GLASS_WORKSHOP_STATE, 10
	subvar VAR_ASH_GATHER_COUNT, BLUE_FLUTE_PRICE
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem
	end

Route113_GlassWorkshop_EventScript_YellowFlute:: @ 816369C
	setvar VAR_0x8008, ITEM_YELLOW_FLUTE
	bufferitemname 0, VAR_0x8008
	setvar VAR_0x800A, YELLOW_FLUTE_PRICE
	compare VAR_ASH_GATHER_COUNT, YELLOW_FLUTE_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	setvar VAR_GLASS_WORKSHOP_STATE, 11
	subvar VAR_ASH_GATHER_COUNT, YELLOW_FLUTE_PRICE
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem
	end

Route113_GlassWorkshop_EventScript_RedFlute:: @ 81636D8
	setvar VAR_0x8008, ITEM_RED_FLUTE
	bufferitemname 0, VAR_0x8008
	setvar VAR_0x800A, RED_FLUTE_PRICE
	compare VAR_ASH_GATHER_COUNT, RED_FLUTE_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	setvar VAR_GLASS_WORKSHOP_STATE, 12
	subvar VAR_ASH_GATHER_COUNT, RED_FLUTE_PRICE
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem
	end

Route113_GlassWorkshop_EventScript_WhiteFlute:: @ 8163714
	setvar VAR_0x8008, ITEM_WHITE_FLUTE
	bufferitemname 0, VAR_0x8008
	setvar VAR_0x800A, WHITE_FLUTE_PRICE
	compare VAR_ASH_GATHER_COUNT, WHITE_FLUTE_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	setvar VAR_GLASS_WORKSHOP_STATE, 13
	subvar VAR_ASH_GATHER_COUNT, WHITE_FLUTE_PRICE
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem
	end

Route113_GlassWorkshop_EventScript_BlackFlute:: @ 8163750
	setvar VAR_0x8008, ITEM_BLACK_FLUTE
	bufferitemname 0, VAR_0x8008
	setvar VAR_0x800A, BLACK_FLUTE_PRICE
	compare VAR_ASH_GATHER_COUNT, BLACK_FLUTE_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	setvar VAR_GLASS_WORKSHOP_STATE, 14
	subvar VAR_ASH_GATHER_COUNT, BLACK_FLUTE_PRICE
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem
	end

Route113_GlassWorkshop_EventScript_PrettyChair:: @ 816378C
	setvar VAR_0x8009, 1
	setvar VAR_0x8008, DECOR_PRETTY_CHAIR
	bufferdecorationname 0, VAR_0x8008
	setvar VAR_0x800A, PRETTY_CHAIR_PRICE
	compare VAR_ASH_GATHER_COUNT, PRETTY_CHAIR_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	setvar VAR_GLASS_WORKSHOP_STATE, 15
	subvar VAR_ASH_GATHER_COUNT, PRETTY_CHAIR_PRICE
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem
	end

Route113_GlassWorkshop_EventScript_PrettyDesk:: @ 81637CD
	setvar VAR_0x8009, 1
	setvar VAR_0x8008, DECOR_PRETTY_DESK
	bufferdecorationname 0, VAR_0x8008
	setvar VAR_0x800A, PRETTY_DESK_PRICE
	compare VAR_ASH_GATHER_COUNT, PRETTY_DESK_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	setvar VAR_GLASS_WORKSHOP_STATE, 16
	subvar VAR_ASH_GATHER_COUNT, PRETTY_DESK_PRICE
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem
	end

Route113_GlassWorkshop_EventScript_CancelGlassItemSelect:: @ 816380E
	msgbox Route113_GlassWorkshop_Text_AllThatAshButDontWantAnything, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_NotEnoughAsh:: @ 8163818
	setvar VAR_0x800A, LOWEST_ASH_PRICE
	subvar VAR_0x800A, 16456
	buffernumberstring 0, 32778
	msgbox Route113_GlassWorkshop_Text_NotEnoughAshNeedX, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_NotEnoughAshForItem:: @ 8163830
	subvar VAR_0x800A, 16456
	buffernumberstring 1, 32778
	message Route113_GlassWorkshop_Text_NotEnoughAshToMakeItem
	waitmessage
	goto Route113_GlassWorkshop_EventScript_ChooseGlassItem
	end

Route113_GlassWorkshop_EventScript_ChooseDifferentItem:: @ 8163845
	message Route113_GlassWorkshop_Text_WhichWouldYouLike
	waitmessage
	goto Route113_GlassWorkshop_EventScript_ChooseGlassItem
	end

Route113_GlassWorkshop_EventScript_MakeGlassItem:: @ 8163851
	msgbox Route113_GlassWorkshop_Text_IllMakeItemForYou, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	playse SE_SELECT
	delay 30
	fadescreen FADE_FROM_BLACK
	msgbox Route113_GlassWorkshop_Text_IveFinishedGlassItem, MSGBOX_DEFAULT
	compare VAR_0x8009, 0
	call_if_eq Route113_GlassWorkshop_EventScript_GiveGlassFlute
	compare VAR_0x8009, 1
	call_if_eq Route113_GlassWorkshop_EventScript_GiveGlassDecor
	setvar VAR_GLASS_WORKSHOP_STATE, 2
	release
	end

Route113_GlassWorkshop_EventScript_GiveGlassFlute:: @ 8163889
	giveitem VAR_0x8008
	compare VAR_RESULT, 0
	goto_if_eq Route113_GlassWorkshop_EventScript_NoRoomForFlute
	return

Route113_GlassWorkshop_EventScript_GiveGlassDecor:: @ 81638A1
	givedecoration VAR_0x8008
	compare VAR_RESULT, 0
	goto_if_eq Route113_GlassWorkshop_EventScript_NoRoomForDecor
	return

Route113_GlassWorkshop_EventScript_NoRoomForFlute:: @ 81638B4
	call Common_EventScript_BagIsFull
	msgbox Route113_GlassWorkshop_Text_NoRoomInBag, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_NoRoomForDecor:: @ 81638C3
	call Common_EventScript_NoRoomForDecor
	msgbox Route113_GlassWorkshop_Text_NoRoomInPC, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_GiveItemAfterNoRoom:: @ 81638D2
	switch VAR_GLASS_WORKSHOP_STATE
	case 10, Route113_GlassWorkshop_EventScript_GiveBlueFlute
	case 11, Route113_GlassWorkshop_EventScript_GiveYellowFlute
	case 12, Route113_GlassWorkshop_EventScript_GiveRedFlute
	case 13, Route113_GlassWorkshop_EventScript_GiveWhiteFlute
	case 14, Route113_GlassWorkshop_EventScript_GiveBlackFlute
	case 15, Route113_GlassWorkshop_EventScript_GivePrettyChair
	case 16, Route113_GlassWorkshop_EventScript_GivePrettyDesk
	end

Route113_GlassWorkshop_EventScript_GiveBlueFlute:: @ 8163925
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_BLUE_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveYellowFlute:: @ 8163939
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_YELLOW_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveRedFlute:: @ 816394D
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_RED_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveWhiteFlute:: @ 8163961
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_WHITE_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveBlackFlute:: @ 8163975
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_BLACK_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GivePrettyChair:: @ 8163989
	setvar VAR_0x8009, 1
	setvar VAR_0x8008, DECOR_PRETTY_CHAIR
	bufferdecorationname 0, DECOR_PRETTY_CHAIR
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GivePrettyDesk:: @ 816399D
	setvar VAR_0x8009, 1
	setvar VAR_0x8008, DECOR_PRETTY_DESK
	bufferdecorationname 0, DECOR_PRETTY_DESK
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_TryGiveItemAgain:: @ 81639B1
	msgbox Route113_GlassWorkshop_Text_IveFinishedGlassItem, MSGBOX_DEFAULT
	compare VAR_0x8009, 0
	call_if_eq Route113_GlassWorkshop_EventScript_GiveGlassFlute
	compare VAR_0x8009, 1
	call_if_eq Route113_GlassWorkshop_EventScript_GiveGlassDecor
	setvar VAR_GLASS_WORKSHOP_STATE, 2
	release
	end

Route113_GlassWorkshop_EventScript_NinjaBoy:: @ 81639D6
	msgbox Route113_GlassWorkshop_Text_FunToBlowGlassFlute, MSGBOX_NPC
	end
