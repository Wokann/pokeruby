Roulette_EventScript_Table1:: @ 81C407E
	checkitem ITEM_COIN_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 0
	getpricereduction 2
	compare VAR_RESULT, 0
	goto_if_eq Roulette_EventScript_Play
	addvar VAR_0x8004, 128
	goto Roulette_EventScript_Play
	end

Roulette_EventScript_Table2:: @ 81C40AC
	checkitem ITEM_COIN_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 1
	getpricereduction 2
	compare VAR_RESULT, 0
	goto_if_eq Roulette_EventScript_Play
	addvar VAR_0x8004, 128
	goto Roulette_EventScript_Play
	end

Roulette_EventScript_Play:: @ 81C40DA
	special PlayRoulette
	waitstate
	end
