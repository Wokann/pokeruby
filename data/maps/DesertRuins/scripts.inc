DesertRuins_MapScripts:: @ 815CAF3
	map_script MAP_SCRIPT_ON_RESUME, DesertRuins_OnResume
	map_script MAP_SCRIPT_ON_LOAD, DesertRuins_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, DesertRuins_OnTransition
	.byte 0

DesertRuins_OnResume:: @ 815CB03
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, DesertRuins_EventScript_TryRemoveRegirock
	end

DesertRuins_EventScript_TryRemoveRegirock:: @ 815CB0D
	removeobject VAR_LAST_TALKED
	return

DesertRuins_OnLoad:: @ 815CB11
	call_if_unset FLAG_SYS_REGIROCK_PUZZLE_COMPLETED, DesertRuins_EventScript_HideRegiEntrance
	end

DesertRuins_EventScript_HideRegiEntrance:: @ 815CB1B
	setmetatile 7, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 8, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 9, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 7, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 8, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 9, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	return

DesertRuins_OnTransition:: @ 815CB52
	setflag FLAG_LANDMARK_DESERT_RUINS
	end

DesertRuins_EventScript_CaveEntranceMiddle:: @ 815CB56
	lockall
	goto_if_set FLAG_SYS_REGIROCK_PUZZLE_COMPLETED, DesertRuins_EventScript_BigHoleInWall
	braillemessage DesertRuins_Braille_UseStrength
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

DesertRuins_EventScript_BigHoleInWall:: @ 815CB6D
	msgbox DesertRuins_Text_1A13BE, MSGBOX_DEFAULT
	releaseall
	end

DesertRuins_EventScript_CaveEntranceSide:: @ 815CB77
	lockall
	braillemessage DesertRuins_Braille_UseStrength
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

DesertRuins_EventScript_Regirock:: @ 815CB85
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGIROCK, 2
	delay 40
	waitmoncry
	setwildbattle SPECIES_REGIROCK, 40, ITEM_NONE
	setflag FLAG_HIDE_REGIROCK
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartRegiBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	release
	end
