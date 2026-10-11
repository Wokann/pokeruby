Roulette_EventScript_Table1:: @ 81C407E
	checkitem ITEM_COIN_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 0
	getpokenewsactive 2
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
	getpokenewsactive 2
	compare VAR_RESULT, 0
	goto_if_eq Roulette_EventScript_Play
	addvar VAR_0x8004, 128
	goto Roulette_EventScript_Play
	end

Roulette_EventScript_Play:: @ 81C40DA
	special PlayRoulette
	waitstate
	end

Roulette_Text_PlayMinimumWagerIsX:: @ 81C40DF
	.string "The minimum wager at this table is {STR_VAR_1}. \n"
	.string "Do you want to play?$"

Roulette_Text_NotEnoughCoins:: @ 81C411C
	.string "You don't have enough COINS.$"

Roulette_Text_SpecialRateTable:: @ 81C4139
	.string "Special rate table right now!$"

Roulette_Text_ControlsInstruction:: @ 81C4157
	.string "Place your wager with the + Control Pad,\n"
	.string "then press the A Button.$"

Roulette_Text_ItsAHit:: @ 81C4199
	.string "It's a hit!$"

Roulette_Text_Jackpot:: @ 81C41A5
	.string "Jackpot!$"

Roulette_Text_NothingDoing:: @ 81C41AE
	.string "Nothing doing!$"

Roulette_Text_YouveWonXCoins:: @ 81C41BD
	.string "You've won {STR_VAR_1} COINS!$"

Roulette_Text_NoCoinsLeft:: @ 81C41D2
	.string "No COINS left...$"

Roulette_Text_KeepPlaying:: @ 81C41E3
	.string "Keep playing?$"

Roulette_Text_BoardWillBeCleared:: @ 81C41F1
	.string "The ROULETTE board will be cleared.$"

Roulette_Text_YouDontHaveACoinCase: @ 81C4215
	.string "You don't have a COIN CASE.$"

Roulette_Text_CoinCaseIsFull:: @ 81C4231
	.string "Your COIN CASE is full!\n"
	.string "Coins can be exchanged for prizes.$"
