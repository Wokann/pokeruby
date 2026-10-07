MauvilleCity_GameCorner_EventScript_1C407E:: @ 81C407E
	checkitem ITEM_COIN_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 0
	getpokenewsactive 2
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_GameCorner_EventScript_1C40DA
	addvar VAR_0x8004, 128
	goto MauvilleCity_GameCorner_EventScript_1C40DA
	end

MauvilleCity_GameCorner_EventScript_1C40AC:: @ 81C40AC
	checkitem ITEM_COIN_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 1
	getpokenewsactive 2
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_GameCorner_EventScript_1C40DA
	addvar VAR_0x8004, 128
	goto MauvilleCity_GameCorner_EventScript_1C40DA
	end

MauvilleCity_GameCorner_EventScript_1C40DA:: @ 81C40DA
	special PlayRoulette
	waitstate
	end

Roulette_Text_PlayMinimumWagerIsX:: @ 81C40DF
	.string "Der Einsatz an diesem Tisch ist\n"
	.string "{STR_VAR_1}. Möchtest du spielen?$"

Roulette_Text_NotEnoughCoins:: @ 81C411C
	.string "Du hast nicht genug MÜNZEN.$"

Roulette_Text_SpecialRateTable:: @ 81C4139
	.string "Und nun eine Bonusrunde!$"

Roulette_Text_ControlsInstruction:: @ 81C4157
	.string "Mache deinen Einsatz mit dem + Steuer-\n"
	.string "kreuz und drücke dann den A-Knopf.$"

Roulette_Text_ItsAHit:: @ 81C4199
	.string "Ein Treffer!$"

Roulette_Text_Jackpot:: @ 81C41A5
	.string "Jackpot!$"

Roulette_Text_NothingDoing:: @ 81C41AE
	.string "Das war nichts!$"

Roulette_Text_YouveWonXCoins:: @ 81C41BD
	.string "Du hast {STR_VAR_1} MÜNZEN gewonnen!$"

Roulette_Text_NoCoinsLeft:: @ 81C41D2
	.string "Nicht mehr genug MÜNZEN übrig...$"

Roulette_Text_KeepPlaying:: @ 81C41E3
	.string "Weiterspielen?$"

Roulette_Text_BoardWillBeCleared:: @ 81C41F1
	.string "Der ROULETTE-Tisch wird abgeräumt.$"

Roulette_Text_YouDontHaveACoinCase: @ 81C4215
	.string "Du hast keinen MÜNZKORB.$"

Roulette_Text_CoinCaseIsFull:: @ 81C4231
	.string "Dein MÜNZKORB ist voll! Münzen können\n"
	.string "gegen Preise eingetauscht werden.$"
