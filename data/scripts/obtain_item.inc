.set ITEMID, VAR_0x8000
.set AMOUNT, VAR_0x8001

Std_ObtainItem: @ 819FDF7
	additem ITEMID, AMOUNT
	copyvar VAR_0x8007, VAR_RESULT
	call EventScript_ObtainItemMessage
	return

EventScript_ObtainItemMessage: @ 819FE07
	bufferitemname STR_VAR_2, ITEMID
	checkitemtype ITEMID
	call EventScript_BufferPocketNameAndTryFanfare
	call_if_eq VAR_0x8007, TRUE, EventScript_ObtainedItem
	call_if_eq VAR_0x8007, FALSE, EventScript_NoRoomForItem
	return

EventScript_BufferPocketNameAndTryFanfare:
	switch VAR_RESULT
	case POCKET_ITEMS,	    EventScript_BufferItemsPocket
	case POCKET_KEY_ITEMS,  EventScript_BufferKeyItemsPocket
	case POCKET_POKE_BALLS, EventScript_BufferPokeballsPocket
	case POCKET_TM_HM,	    EventScript_BufferTMHMsPocket
	case POCKET_BERRIES,	EventScript_BufferBerriesPocket
	end

EventScript_BufferItemsPocket:
	bufferstdstring STR_VAR_3, STDSTRING_ITEMS
	call_if_eq VAR_0x8007, TRUE, EventScript_PlayFanfareObtainedItem
	return

EventScript_BufferKeyItemsPocket:
	bufferstdstring STR_VAR_3, STDSTRING_KEYITEMS
	call_if_eq VAR_0x8007, TRUE, EventScript_PlayFanfareObtainedItem
	return

EventScript_BufferPokeballsPocket:
	bufferstdstring STR_VAR_3, STDSTRING_POKEBALLS
	call_if_eq VAR_0x8007, TRUE, EventScript_PlayFanfareObtainedItem
	return

EventScript_BufferTMHMsPocket:
	bufferstdstring STR_VAR_3, STDSTRING_TMHMS
	call_if_eq VAR_0x8007, TRUE, EventScript_PlayFanfareObtainedTMHM
	return

EventScript_BufferBerriesPocket:
	bufferstdstring STR_VAR_3, STDSTRING_BERRIES
	call_if_eq VAR_0x8007, TRUE, EventScript_PlayFanfareObtainedItem
	return

EventScript_ObtainedItem: @ 819FEB7
	message gText_ObtainedTheItem
	waitfanfare
	waitmessage
	msgbox gText_PutItemInPocket, MSGBOX_DEFAULT
	setvar VAR_RESULT, TRUE
	return

EventScript_NoRoomForItem: @ 819FECC
	setvar VAR_RESULT, FALSE
	return

EventScript_PlayFanfareObtainedItem:
	playfanfare MUS_OBTAIN_ITEM
	return

EventScript_PlayFanfareObtainedTMHM:
	playfanfare MUS_OBTAIN_TMHM
	return

Std_ObtainDecoration: @ 819FEDA
	adddecoration ITEMID
	copyvar VAR_0x8007, VAR_RESULT
	call EventScript_ObtainDecorationMessage
	return

EventScript_ObtainDecorationMessage: @ 819FEE8
	bufferdecorationname STR_VAR_2, ITEMID
	call_if_eq VAR_0x8007, TRUE, EventScript_ObtainedDecor
	call_if_eq VAR_0x8007, FALSE, EventScript_NoRoomForDecor
	return

EventScript_ObtainedDecor: @ 819FF03
	playfanfare MUS_OBTAIN_ITEM
	message gText_ObtainedTheDecor
	waitfanfare
	waitmessage
	msgbox gText_TheDecorWasTransferredToThePC, MSGBOX_DEFAULT
	setvar VAR_RESULT, TRUE
	return

EventScript_NoRoomForDecor: @ 819FF1B
	setvar VAR_RESULT, FALSE
	return

Std_FindItem: @ 819FF21
	lock
	faceplayer
	waitse
	additem ITEMID, AMOUNT
	copyvar VAR_0x8007, VAR_RESULT
	bufferitemname STR_VAR_2, ITEMID
	checkitemtype ITEMID
	call EventScript_BufferPocketNameAndTryFanfare
	call_if_eq VAR_0x8007, TRUE, EventScript_PickUpItem
	call_if_eq VAR_0x8007, FALSE, EventScript_NoRoomToPickUpItem
	release
	return

EventScript_PickUpItem: @ 819FF52
	removeobject VAR_LAST_TALKED
	message gText_PlayerFoundOneItem
	waitfanfare
	waitmessage
	msgbox gText_PutItemInPocket, MSGBOX_DEFAULT
	return

EventScript_NoRoomToPickUpItem: @ 819FF65
	msgbox gText_ObtainedTheItem, MSGBOX_DEFAULT
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	setvar VAR_RESULT, FALSE
	return

EventScript_HiddenItemScript:: @ 819FF7B
	lockall
	waitse
	additem VAR_0x8005
	copyvar VAR_0x8007, VAR_RESULT
	bufferitemname STR_VAR_2, VAR_0x8005
	checkitemtype VAR_0x8005
	call EventScript_BufferPocketNameAndTryFanfare
	goto_if_eq VAR_0x8007, TRUE, EventScript_PickUpHiddenItem
	goto_if_eq VAR_0x8007, FALSE, EventScript_NoRoomToPickUpHiddenItem
	end

EventScript_PickUpHiddenItem:
	message gText_PlayerFoundOneItem
	waitfanfare
	waitmessage
	msgbox gText_PutItemInPocket, MSGBOX_DEFAULT
	special SetHiddenItemFlag
	releaseall
	end

EventScript_NoRoomToPickUpHiddenItem:
	msgbox gText_PlayerFoundOneItem, MSGBOX_DEFAULT
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	setvar VAR_RESULT, FALSE
	releaseall
	end
