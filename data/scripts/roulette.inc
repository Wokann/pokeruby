	.equiv POKENEWS_GAME_CORNER, 2
	.equiv ROULETTE_SPECIAL_RATE, 1 << 7

Roulette_EventScript_Table1:: @ 81C407E
	checkitem ITEM_COIN_CASE, 1
	compare VAR_RESULT, FALSE
	goto_if_eq MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 0
	getpricereduction POKENEWS_GAME_CORNER
	compare VAR_RESULT, FALSE
	goto_if_eq Roulette_EventScript_Play
	addvar VAR_0x8004, ROULETTE_SPECIAL_RATE
	goto Roulette_EventScript_Play
	end

Roulette_EventScript_Table2:: @ 81C40AC
	checkitem ITEM_COIN_CASE, 1
	compare VAR_RESULT, FALSE
	goto_if_eq MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 1
	getpricereduction POKENEWS_GAME_CORNER
	compare VAR_RESULT, FALSE
	goto_if_eq Roulette_EventScript_Play
	addvar VAR_0x8004, ROULETTE_SPECIAL_RATE
	goto Roulette_EventScript_Play
	end

Roulette_EventScript_Play:: @ 81C40DA
	special PlayRoulette
	waitstate
	end

	.include "data/text/roulette.inc"
