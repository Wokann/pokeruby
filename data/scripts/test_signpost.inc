EventScript_TestNpcMessage:: @ 81C6BF9
	msgbox Text_ThisIsATestMessage, MSGBOX_NPC
	end

EventScript_TestSignpostMsg:: @ 81C6C02
	msgbox Text_ThisIsATestSignpostMsg, MSGBOX_SIGN
	end

EventScript_TestEmpty:: @ 81C6C0B
	end

EventScript_TestCoordEvent:: @ 81C6C0C
	msgbox Text_ThisIsATestCoordEvent, MSGBOX_SIGN
	end

EventScript_TestSetWallClock:: @ 81C6C15
	lockall
	call PlayersHouse_2F_EventScript_SetWallClock
	releaseall
	end

EventScript_TestBrailleMessage:: @ 81C6C1D
	lockall
	braillemessage Underwater_SealedChamber_Braille_GoUpHere
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

Text_ThisIsATestMessage:: @ 81C6C2B
	@ This is a test message!
	@ Welcome to the world of Pokémon!
	.string "テストよう　メッセージです！\n"
	.string "ポケモンの　せかいへ　ようこそ！$"

Text_ThisIsATestSignpostMsg:: @ 81C6C4B
	@ This is a test message!
	@ This is a sign.
	.string "テストよう　メッセージです！\n"
	.string "かんばん　です$"

Text_ThisIsATestCoordEvent:: @ 81C6C62
	@ This is a test message!
	@ This is a coordinate-check event.
	.string "テストよう　メッセージです！\n"
	.string "ざひょう　チェックの　イベントです$"

@ 81C6C84
@ object file boundary?
	.align 2
	.string "$"
