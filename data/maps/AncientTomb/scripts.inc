AncientTomb_MapScripts:: @ 815EFBE
	map_script MAP_SCRIPT_ON_RESUME, AncientTomb_OnResume
	map_script MAP_SCRIPT_ON_LOAD, AncientTomb_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, AncientTomb_OnTransition
	.byte 0

AncientTomb_OnResume:: @ 815EFCE
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, AncientTomb_EventScript_TryRemoveRegisteel
	end

AncientTomb_EventScript_TryRemoveRegisteel:: @ 815EFD8
	removeobject VAR_LAST_TALKED
	return

AncientTomb_OnTransition:: @ 815EFDC
	setflag FLAG_LANDMARK_ANCIENT_TOMB
	end

AncientTomb_OnLoad:: @ 815EFE0
	call_if_unset FLAG_SYS_BRAILLE_FLY, AncientTomb_EventScript_HideRegiEntrance
	end

AncientTomb_EventScript_HideRegiEntrance:: @ 815EFEA
	setmetatile 7, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 8, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 9, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 7, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 8, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 9, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	return

AncientTomb_EventScript_CaveEntranceMiddle:: @ 815F021
	lockall
	goto_if_set FLAG_SYS_BRAILLE_FLY, AncientTomb_EventScript_BigHoleInWall
	braillemessage AncientTomb_Braille_AimToTheSkyInTheMiddle
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

AncientTomb_EventScript_BigHoleInWall:: @ 815F038
	msgbox gText_BigHoleInTheWall, MSGBOX_DEFAULT
	releaseall
	end

AncientTomb_EventScript_CaveEntranceSide:: @ 815F042
	lockall
	braillemessage AncientTomb_Braille_AimToTheSkyInTheMiddle
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end

AncientTomb_EventScript_Registeel:: @ 815F050
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGISTEEL, 2
	delay 40
	waitmoncry
	setwildbattle SPECIES_REGISTEEL, 40, ITEM_NONE
	setflag FLAG_HIDE_REGISTEEL
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartRegiBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	release
	end
