SealedChamber_InnerRoom_MapScripts:: @ 815F1E7
	.byte 0

SealedChamber_InnerRoom_EventScript_BrailleBackWall:: @ 815F1E8
	lockall
	braillemessage SealedChamber_InnerRoom_Braille_FirstRelicanthLastWailord
	waitbuttonpress
	erasebox 0, 0, 29, 19
	goto_if_set FLAG_REGI_DOORS_OPENED, SealedChamber_InnerRoom_EventScript_NoEffect
	specialvar VAR_RESULT, CheckRelicanthWailord
	compare VAR_RESULT, 0
	goto_if_eq SealedChamber_InnerRoom_EventScript_NoEffect
	fadeoutbgm 0
	playse SE_TRUCK_MOVE
	special DoSealedChamberShakingEffect1
	waitstate
	delay 40
	special DoSealedChamberShakingEffect2
	waitstate
	playse SE_DOOR
	delay 40
	special DoSealedChamberShakingEffect2
	waitstate
	playse SE_DOOR
	delay 40
	special DoSealedChamberShakingEffect2
	waitstate
	playse SE_DOOR
	delay 40
	msgbox SealedChamber_InnerRoom_Text_DoorOpenedFarAway, MSGBOX_DEFAULT
	closemessage
	fadeinbgm 0
	setflag FLAG_REGI_DOORS_OPENED
	releaseall
	end

SealedChamber_InnerRoom_EventScript_NoEffect:: @ 815F247
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart1:: @ 815F249
	lockall
	braillemessage SealedChamber_InnerRoom_Braille_InThisCaveWeHaveLived
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart2:: @ 815F257
	lockall
	braillemessage SealedChamber_InnerRoom_Braille_WeOweAllToThePokemon
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart3:: @ 815F265
	lockall
	braillemessage SealedChamber_InnerRoom_Braille_ButWeSealedThePokemonAway
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart4:: @ 815F273
	lockall
	braillemessage SealedChamber_InnerRoom_Braille_WeFearedIt
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart5:: @ 815F281
	lockall
	braillemessage SealedChamber_InnerRoom_Braille_ThoseWithCourageHope
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart6:: @ 815F28F
	lockall
	braillemessage SealedChamber_InnerRoom_Braille_OpenDoorEternalPokemonWaits
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end
