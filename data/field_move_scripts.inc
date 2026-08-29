EventScript_CutTree:: @ 81B0DCC
	lockall
	goto_if_unset FLAG_BADGE01_GET, EventScript_CheckTreeCantCut
	checkpartymove MOVE_CUT
	compare VAR_RESULT, PARTY_SIZE
	goto_if_eq EventScript_CheckTreeCantCut
	setfieldeffectargument 0, VAR_RESULT
	bufferpartymonnick STR_VAR_1, VAR_RESULT
	buffermovename STR_VAR_2, MOVE_CUT
	msgbox Text_WantToCut, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_CancelCut
	msgbox Text_MonUsedFieldMove, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_CUT_ON_TREE
	waitstate
	goto EventScript_CutTreeDown
	end

EventScript_UseCut:: @ 81B0E16
	lockall
	dofieldeffect FLDEFF_USE_CUT_ON_TREE
	waitstate
	goto EventScript_CutTreeDown
	end

EventScript_CutTreeDown: @ 81B0E21
	applymovement VAR_LAST_TALKED, Movement_CutTreeDown @ tree cut animation
	waitmovement 0
	removeobject VAR_LAST_TALKED @ tree disappears
	releaseall
	end

Movement_CutTreeDown: @ 81B0E30
	cut_tree
	step_end

EventScript_CheckTreeCantCut: @ 81B0E32
	msgbox Text_CantCut, MSGBOX_SIGN
	releaseall
	end

EventScript_CancelCut: @ 81B0E3C
	closemessage
	releaseall
	end

Text_WantToCut: @ 81B0E3F
	.string "This tree looks like it can be CUT down.\p"
	.string "Would you like to CUT it?$"

Text_MonUsedFieldMove: @ 81B0E82
	.string "{STR_VAR_1} used {STR_VAR_2}.$"

Text_CantCut: @ 81B0E8E
	.string "This tree looks like it can be CUT down.$"

@ Interact with smashable rock
EventScript_RockSmash:: @ 81B0EB7
	lockall
	goto_if_unset FLAG_BADGE03_GET, EventScript_CantSmashRock
	checkpartymove MOVE_ROCK_SMASH
	goto_if_eq VAR_RESULT, PARTY_SIZE, EventScript_CantSmashRock
	setfieldeffectargument 0, VAR_RESULT
	bufferpartymonnick STR_VAR_1, VAR_RESULT
	buffermovename STR_VAR_2, MOVE_ROCK_SMASH
	msgbox Text_WantToSmash, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_CancelSmash
	msgbox Text_MonUsedFieldMove, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_ROCK_SMASH
	waitstate
	goto EventScript_SmashRock
	end

@ Use rock smash from party menu
EventScript_UseRockSmash:: @ 81B0F01
	lockall
	dofieldeffect FLDEFF_USE_ROCK_SMASH
	waitstate
	goto EventScript_SmashRock
	end

EventScript_SmashRock:: @ 81B0F0C
	applymovement VAR_LAST_TALKED, Movement_SmashRock
	waitmovement 0
	removeobject VAR_LAST_TALKED
	specialvar VAR_RESULT, TryUpdateRusturfTunnelState
	goto_if_eq VAR_RESULT, TRUE, EventScript_EndSmash
	special RockSmashWildEncounter
	goto_if_eq VAR_RESULT, FALSE, EventScript_EndSmash
	waitstate
	releaseall
	end

EventScript_EndSmash:: @ 81B0F3A
	releaseall
	end

Movement_SmashRock: @ 81B0F3C
	rock_smash_break
	step_end

EventScript_CantSmashRock:: @ 81B0F3E
	msgbox Text_CantSmash, MSGBOX_SIGN
	releaseall
	end

EventScript_CancelSmash:: @ 81B0F48
	closemessage
	releaseall
	end

Text_WantToSmash: @ 81B0F4B
	.string "This rock appears to be breakable.\n"
	.string "Would you like to use ROCK SMASH?$"

Text_CantSmash: @ 81B0F90
	.string "It's a rugged rock, but a POKéMON\n"
	.string "may be able to smash it.$"

EventScript_StrengthBoulder:: @ 81B0FCB
	lockall
	goto_if_unset FLAG_BADGE04_GET, EventScript_CantStrength
	goto_if_set FLAG_SYS_USE_STRENGTH, EventScript_CheckActivatedBoulder
	checkpartymove MOVE_STRENGTH
	compare VAR_RESULT, PARTY_SIZE
	goto_if_eq EventScript_CantStrength
	setfieldeffectargument 0, VAR_RESULT
	msgbox Text_WantToStrength, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_CancelStrength
	closemessage
	dofieldeffect FLDEFF_USE_STRENGTH
	waitstate
	goto EventScript_ActivateStrength
	end

EventScript_UseStrength:: @ 81B100E
	lockall
	dofieldeffect FLDEFF_USE_STRENGTH
	waitstate
	goto EventScript_ActivateStrength
	end

EventScript_ActivateStrength: @ 81B1019
	setflag FLAG_SYS_USE_STRENGTH
	msgbox Text_MonUsedStrength, MSGBOX_SIGN
	releaseall
	end

EventScript_CantStrength: @ 81B1026
	msgbox Text_CantStrength, MSGBOX_SIGN
	releaseall
	end

EventScript_CheckActivatedBoulder: @ 81B1030
	msgbox Text_StrengthActivated, MSGBOX_SIGN
	releaseall
	end

EventScript_CancelStrength: @ 81B103A
	closemessage
	releaseall
	end

Text_WantToStrength: @ 81B103D
	.string "It's a big boulder, but a POKéMON\n"
	.string "may be able to push it aside.\p"
	.string "Would you like to use STRENGTH?$"

Text_MonUsedStrength: @ 81B109D
	.string "{STR_VAR_1} used STRENGTH.\p"
	.string "{STR_VAR_1}'s STRENGTH made it possible\n"
	.string "to move boulders around.$"

Text_CantStrength: @ 81B10E7
	.string "It's a big boulder, but a POKéMON\n"
	.string "may be able to push it aside.$"

Text_StrengthActivated: @ 81B1127
	.string "STRENGTH made it possible to move\n"
	.string "boulders around.$"

EventScript_UseWaterfall:: @ 81B115A
	lockall
	checkpartymove MOVE_WATERFALL
	compare VAR_RESULT, PARTY_SIZE
	goto_if_eq EventScript_CantWaterfall
	bufferpartymonnick STR_VAR_1, VAR_RESULT
	setfieldeffectargument 0, VAR_RESULT
	msgbox Text_WantToWaterfall, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_EndWaterfall
	msgbox Text_MonUsedWaterfall, MSGBOX_DEFAULT
	dofieldeffect FLDEFF_USE_WATERFALL
	goto EventScript_EndWaterfall

EventScript_CannotUseWaterfall:: @ 81B1194
	lockall
EventScript_CantWaterfall: @ 81B1195
	msgbox Text_CantWaterfall, MSGBOX_DEFAULT
EventScript_EndWaterfall:  @ 81B119D
	releaseall
	end

Text_CantWaterfall: @ 81B119F
	.string "A wall of water is crashing down with\n"
	.string "a mighty roar.$"

Text_WantToWaterfall: @ 81B11D4
	.string "It's a large waterfall.\n"
	.string "Would you like to use WATERFALL?$"

Text_MonUsedWaterfall: @ 81B120D
	.string "{STR_VAR_1} used WATERFALL.$"

EventScript_UseDive:: @ 81B1220
	lockall
	checkpartymove MOVE_DIVE
	compare VAR_RESULT, PARTY_SIZE
	goto_if_eq EventScript_CantDive
	bufferpartymonnick STR_VAR_1, VAR_RESULT
	setfieldeffectargument 0, VAR_RESULT
	setfieldeffectargument 1, 1
	msgbox Text_WantToDive, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_EndDive
	msgbox Text_MonUsedDive, MSGBOX_DEFAULT
	dofieldeffect FLDEFF_USE_DIVE
	goto EventScript_EndDive
	lockall

EventScript_CantDive: @ 81B125F
	msgbox Text_CantDive, MSGBOX_DEFAULT

EventScript_EndDive: @ 81B1267
	releaseall
	end

EventScript_UseDiveUnderwater:: @ 81B1269
	lockall
	checkpartymove MOVE_DIVE
	compare VAR_RESULT, PARTY_SIZE
	goto_if_eq EventScript_CantSurface
	bufferpartymonnick STR_VAR_1, VAR_RESULT
	setfieldeffectargument 0, VAR_RESULT
	setfieldeffectargument 1, 1
	msgbox Text_WantToSurface, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_EndSurface
	msgbox Text_MonUsedDive, MSGBOX_DEFAULT
	dofieldeffect FLDEFF_USE_DIVE
	goto EventScript_EndSurface

EventScript_CantSurface: @ 81B12A7
	lockall
	msgbox Text_CantSurface, MSGBOX_DEFAULT
	goto EventScript_EndSurface

@ not used
EventScript_SurfacingBlocked: @ 81B12B5
	lockall
	msgbox Text_SurfacingBlocked, MSGBOX_DEFAULT

EventScript_EndSurface: @ 81B12BE
	releaseall
	end

Text_CantDive: @ 81B12C0
	.string "The sea is deep here. A POKéMON may be\n"
	.string "able to go underwater here.$"

Text_WantToDive: @ 81B1303
	.string "The sea is deep here.\n"
	.string "Would you like to use DIVE?$"

Text_MonUsedDive: @ 81B1335
	.string "{STR_VAR_1} used DIVE.$"

Text_CantSurface: @ 81B1343
	.string "Light is filtering down from above.\n"
	.string "A POKéMON may be able to surface here.$"

Text_WantToSurface: @ 81B138E
	.string "Light is filtering down from above.\n"
	.string "Would you like to use DIVE?$"

Text_SurfacingBlocked: @ 81B13CE
	.string "There is something in the way above.\n"
	.string "DIVE can't be used here.$"

EventScript_FailSweetScent:: @ 81B140C
	msgbox Text_FailSweetScent, MSGBOX_SIGN
	end

Text_FailSweetScent: @ 81B1415
	.string "There appears to be nothing here...$"
