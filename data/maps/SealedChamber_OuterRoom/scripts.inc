SealedChamber_OuterRoom_MapScripts:: @ 815F0C6
	map_script MAP_SCRIPT_ON_RESUME, SealedChamber_OuterRoom_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, SealedChamber_OuterRoom_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, SealedChamber_OuterRoom_OnLoad
	.byte 0

SealedChamber_OuterRoom_OnResume:: @ 815F0D6
	setdivewarp MAP_UNDERWATER_SEALED_CHAMBER, WARP_ID_NONE, 12, 44
	setescapewarp MAP_UNDERWATER_SEALED_CHAMBER, 255, 12, 44
	end

SealedChamber_OuterRoom_OnTransition:: @ 815F0E7
	setflag FLAG_LANDMARK_SEALED_CHAMBER
	end

SealedChamber_OuterRoom_OnLoad:: @ 815F0EB
	call_if_unset FLAG_SYS_BRAILLE_DIG, SealedChamber_OuterRoom_EventScript_CloseInnerRoomEntrance
	end

SealedChamber_OuterRoom_EventScript_CloseInnerRoomEntrance:: @ 815F0F5
	setmetatile 9, 1, METATILE_Cave_EntranceCover, 1
	setmetatile 10, 1, METATILE_Cave_EntranceCover, 1
	setmetatile 11, 1, METATILE_Cave_EntranceCover, 1
	setmetatile 9, 2, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 10, 2, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 11, 2, METATILE_Cave_SealedChamberBraille_Mid, 1
	return

SealedChamber_OuterRoom_EventScript_BrailleABC:: @ 815F12C
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_ABC
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleGHI:: @ 815F13A
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_GHI
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleMNO:: @ 815F148
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_MNO
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleTUV:: @ 815F156
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_TUV
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleDEF:: @ 815F164
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_DEF
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleJKL:: @ 815F172
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_JKL
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BraillePQRS:: @ 815F180
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_PQRS
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BraillePeriod:: @ 815F18E
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_Period
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleWXYZ:: @ 815F19C
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_WXYZ
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleComma:: @ 815F1AA
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_Comma
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_InnerRoomEntranceWall:: @ 815F1B8
	lockall
	goto_if_set FLAG_SYS_BRAILLE_DIG, SealedChamber_OuterRoom_EventScript_HoleInWall
	braillemessage SealedChamber_OuterRoom_Braille_DigHere
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

SealedChamber_OuterRoom_EventScript_HoleInWall:: @ 815F1CF
	msgbox gText_BigHoleInTheWall, MSGBOX_DEFAULT
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleDigHere:: @ 815F1D9
	lockall
	braillemessage SealedChamber_OuterRoom_Braille_DigHere
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end
