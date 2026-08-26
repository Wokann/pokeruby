IslandCave_MapScripts:: @ 815EEB6
	map_script MAP_SCRIPT_ON_RESUME, IslandCave_OnResume
	map_script MAP_SCRIPT_ON_LOAD, IslandCave_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, IslandCave_OnTransition
	.byte 0

IslandCave_OnResume:: @ 815EEC6
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, IslandCave_EventScript_TryRemoveRegice
	end

IslandCave_EventScript_TryRemoveRegice:: @ 815EED0
	removeobject VAR_LAST_TALKED
	return

IslandCave_OnLoad:: @ 815EED4
	call_if_unset FLAG_SYS_BRAILLE_WAIT, IslandCave_EventScript_HideRegiEntrance
	end

IslandCave_EventScript_HideRegiEntrance:: @ 815EEDE
	setmetatile 7, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 8, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 9, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 7, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 8, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 9, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	return

IslandCave_OnTransition:: @ 815EF15
	setflag FLAG_LANDMARK_ISLAND_CAVE
	end

IslandCave_EventScript_OpenRegiEntrance:: @ 815EF19
	setmetatile 7, 19, METATILE_Cave_SealedChamberEntrance_TopLeft, 1
	setmetatile 8, 19, METATILE_Cave_SealedChamberEntrance_TopMid, 1
	setmetatile 9, 19, METATILE_Cave_SealedChamberEntrance_TopRight, 1
	setmetatile 7, 20, METATILE_Cave_SealedChamberEntrance_BottomLeft, 1
	setmetatile 8, 20, METATILE_Cave_SealedChamberEntrance_BottomMid, 0
	setmetatile 9, 20, METATILE_Cave_SealedChamberEntrance_BottomRight, 1
	special DrawWholeMapView
	playse SE_BANG
	setflag FLAG_SYS_BRAILLE_WAIT
	end

IslandCave_EventScript_CaveEntranceMiddle:: @ 815EF59
	lockall
	goto_if_set FLAG_SYS_BRAILLE_WAIT, IslandCave_EventScript_BigHoleInWall
	braillemessage IslandCave_Braille_WaitTwoMinutes
	special DoBrailleWait
	waitstate
	goto IslandCave_EventScript_CloseBrailleMsg
	end

IslandCave_EventScript_BigHoleInWall:: @ 815EF72
	msgbox gText_BigHoleInTheWall, MSGBOX_DEFAULT
	releaseall
	end

IslandCave_EventScript_CaveEntranceSide:: @ 815EF7C
	lockall
	braillemessage IslandCave_Braille_WaitTwoMinutes
	goto_if_set FLAG_SYS_BRAILLE_WAIT, IslandCave_EventScript_CloseBrailleMsg
	special DoBrailleWait
	waitstate
	goto IslandCave_EventScript_CloseBrailleMsg
	end

IslandCave_EventScript_CloseBrailleMsg:: @ 815EF95
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

IslandCave_EventScript_Regice:: @ 815EF9D
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGICE, 2
	delay 40
	waitmoncry
	setwildbattle SPECIES_REGICE, 40, ITEM_NONE
	setflag FLAG_HIDE_REGICE
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartRegiBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	release
	end
