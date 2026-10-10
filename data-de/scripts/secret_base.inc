SecretBase_Text_TreeCanBeClimbed: @ 81A1948
	.string "Wenn einige Äste herunterhängen, kann\n"
	.string "man auf den Baum klettern.$"

SecretBase_Text_TreeUseSecretPower: @ 81A197B
	.string "Wenn einige Äste herunterhängen, kann\n"
	.string "man auf den Baum klettern.\p"
	.string "Möchtest du GEHEIMPOWER einsetzen?$"

SecretBase_Text_VineDroppedDown: @ 81A19C4
	.string "Ein dicker Ast fällt herunter!$"

UnknownString_81A19DF: @ 81A19DF
	.string "Möchtest du hier deine GEHEIMBASIS\n"
	.string "einrichten?$"

SecretBase_Text_ClumpOfGrass: @ 81A1A03
	.string "Dieser Busch kann bewegt werden,\n"
	.string "so dass man hineinklettern kann.$"

SecretBase_Text_ClumpUseSecretPower: @ 81A1A4B
	.string "Dieser Busch kann bewegt werden,\n"
	.string "so dass man hineinklettern kann.\p"
	.string "Möchtest du GEHEIMPOWER einsetzen?$"

SecretBase_Text_DiscoveredSmallEntrance: @ 81A1AA9
	.string "Ein kleiner Eingang wird sichtbar.$"

UnknownString_81A1AC6: @ 81A1AC6
	.string "Möchtest du hier deine GEHEIMBASIS\n"
	.string "einrichten?$"

	.include "data/text/secret_base_trainers.inc"

SecretBase_EventScript_CheckEntrance:: @ 81A2C51
	special GetSecretBaseTypeInFrontOfPlayer
	special CheckPlayerHasSecretBase
	compare VAR_RESULT, 1
	goto_if_eq AskToMoveSecretBase
	checkpartymove MOVE_SECRET_POWER
	setfieldeffectargument 0, VAR_RESULT
	buffermovename 1, MOVE_SECRET_POWER
	compare VAR_0x8007, 1
	goto_if_eq EventScript_1A2CB0
	compare VAR_0x8007, 2
	goto_if_eq EventScript_1A2CB0
	compare VAR_0x8007, 3
	goto_if_eq EventScript_1A2CB0
	compare VAR_0x8007, 4
	goto_if_eq EventScript_1A2CB0
	compare VAR_0x8007, 5
	goto_if_eq EventScript_1A2D08
	compare VAR_0x8007, 6
	goto_if_eq EventScript_1A2D60
	end

EventScript_1A2CB0:
	lockall
	compare VAR_RESULT, 6
	goto_if_eq EventScript_1A2CF1
	bufferpartymonnick 0, VAR_RESULT
	msgbox SecretBase_Text_IndentUseSecretPower, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_1A2F3A
	msgbox UsedCutRockSmashText, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_SECRET_POWER_CAVE
	waitstate
	goto EventScript_1A2CFA
	end

SecretBase_EventScript_CaveUseSecretPower:: @ 81A2CE6
	lockall
	dofieldeffect FLDEFF_USE_SECRET_POWER_CAVE
	waitstate
	goto EventScript_1A2CFA
	end

EventScript_1A2CF1:
	msgbox SecretBase_Text_SmallIndentInWall, MSGBOX_SIGN
	end

EventScript_1A2CFA:
	msgbox SecretBase_Text_DiscoveredSmallCavern, MSGBOX_DEFAULT
	goto EventScript_1A2DB8
	end

EventScript_1A2D08:
	lockall
	compare VAR_RESULT, 6
	goto_if_eq EventScript_1A2D49
	bufferpartymonnick 0, VAR_RESULT
	msgbox SecretBase_Text_TreeUseSecretPower, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_1A2F3A
	msgbox UsedCutRockSmashText, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_SECRET_POWER_TREE
	waitstate
	goto EventScript_1A2D52
	end

SecretBase_EventScript_TreeUseSecretPower:: @ 81A2D3E
	lockall
	dofieldeffect FLDEFF_USE_SECRET_POWER_TREE
	waitstate
	goto EventScript_1A2D52
	end

EventScript_1A2D49:
	msgbox SecretBase_Text_TreeCanBeClimbed, MSGBOX_SIGN
	end

EventScript_1A2D52:
	msgbox SecretBase_Text_VineDroppedDown, MSGBOX_DEFAULT
	goto EventScript_1A2DB8
	end

EventScript_1A2D60:
	lockall
	compare VAR_RESULT, 6
	goto_if_eq EventScript_1A2DA1
	bufferpartymonnick 0, VAR_RESULT
	msgbox SecretBase_Text_ClumpUseSecretPower, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_1A2F3A
	msgbox UsedCutRockSmashText, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_SECRET_POWER_SHRUB
	waitstate
	goto EventScript_1A2DAA
	end

SecretBase_EventScript_ShrubUseSecretPower:: @ 81A2D96
	lockall
	dofieldeffect FLDEFF_USE_SECRET_POWER_SHRUB
	waitstate
	goto EventScript_1A2DAA
	end

EventScript_1A2DA1:
	msgbox SecretBase_Text_ClumpOfGrass, MSGBOX_SIGN
	end

EventScript_1A2DAA:
	msgbox SecretBase_Text_DiscoveredSmallEntrance, MSGBOX_DEFAULT
	goto EventScript_1A2DB8
	end

EventScript_1A2DB8:
	closemessage
	playse SE_EXIT
	setvar VAR_INIT_SECRET_BASE, 0
	setflag FLAG_DECORATION_1
	special SetPlayerSecretBase
	special EnterSecretBase
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, 0
	special InitSecretBaseDecorationSprites
	setvar VAR_SECRET_BASE_INITIALIZED, 1
	waitstate
	end

SecretBase_EventScript_FirstEntrance:: @ 81A2DDE
	applymovement LOCALID_PLAYER, SecretBase_RedCave1_Movement_1A2E11
	waitmovement 0
	setvar VAR_INIT_SECRET_BASE, 1
	msgbox SecretBase_Text_WantToMakeYourSecretBaseHere, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SecretBase_RedCave1_EventScript_1A2E08
	closemessage
	playse SE_EXIT
	special ClearAndLeaveSecretBase
	end

SecretBase_RedCave1_EventScript_1A2E08:: @ 81A2E08
	closemessage
	setflag FLAG_RECEIVED_SECRET_POWER
	special EnterNewlyCreatedSecretBase
	waitstate
	end

SecretBase_RedCave1_Movement_1A2E11:: @ 81A2E11
	walk_up
	walk_up
	step_end

SecretBase_EventScript_Enter:: @ 81A2E14
	lockall
	setvar VAR_INIT_SECRET_BASE, 1
	playse SE_EXIT
	special IsCurSecretBaseOwnedByAnotherPlayer
	compare VAR_RESULT, 0
	goto_if_eq EventScript_1A2E38
	clearflag FLAG_DECORATION_1
	special EnterSecretBase
	setvar VAR_SECRET_BASE_INITIALIZED, 0
	waitstate
	end

EventScript_1A2E38:
	setflag FLAG_DECORATION_1
	special EnterSecretBase
	setvar VAR_SECRET_BASE_INITIALIZED, 0
	waitstate
	end

AskToMoveSecretBase:
	checkpartymove MOVE_SECRET_POWER
	compare VAR_RESULT, 6
	goto_if_eq EventScript_1A2EF7
	setfieldeffectargument 0, VAR_RESULT
	setorcopyvar VAR_0x8004, VAR_RESULT
	lockall
	special GetSecretBaseNearbyMapName
	msgbox UnknownString_81A3C71, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_1A2F3A
	msgbox SecretBase_Text_AllDecorationsWillBeReturned, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_1A2F3A
	fadescreen FADE_TO_BLACK
	special MoveOutOfSecretBaseFromOutside
	closemessage
	fadescreen FADE_FROM_BLACK
	msgbox UnknownString_81A3CC9, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_1A2F3A
	bufferpartymonnick 0, VAR_0x8004
	buffermovename 1, MOVE_SECRET_POWER
	msgbox UsedCutRockSmashText, MSGBOX_DEFAULT
	closemessage
	closemessage
	compare VAR_0x8007, 1
	goto_if_eq SecretBase_EventScript_CaveUseSecretPower
	compare VAR_0x8007, 2
	goto_if_eq SecretBase_EventScript_CaveUseSecretPower
	compare VAR_0x8007, 3
	goto_if_eq SecretBase_EventScript_CaveUseSecretPower
	compare VAR_0x8007, 4
	goto_if_eq SecretBase_EventScript_CaveUseSecretPower
	compare VAR_0x8007, 5
	goto_if_eq SecretBase_EventScript_TreeUseSecretPower
	compare VAR_0x8007, 6
	goto_if_eq SecretBase_EventScript_ShrubUseSecretPower
	releaseall
	end

EventScript_1A2EF7::
	compare VAR_0x8007, 1
	goto_if_eq EventScript_1A2CF1
	compare VAR_0x8007, 2
	goto_if_eq EventScript_1A2CF1
	compare VAR_0x8007, 3
	goto_if_eq EventScript_1A2CF1
	compare VAR_0x8007, 4
	goto_if_eq EventScript_1A2CF1
	compare VAR_0x8007, 5
	goto_if_eq EventScript_1A2D49
	compare VAR_0x8007, 6
	goto_if_eq EventScript_1A2DA1
	end

EventScript_1A2F3A::
	closemessage
	releaseall
	end

SecretBase_EventScript_SetDecorationFlags:: @ 81A2F3D
	setflag FLAG_DECORATION_2
	setflag FLAG_DECORATION_3
	setflag FLAG_DECORATION_4
	setflag FLAG_DECORATION_5
	setflag FLAG_DECORATION_6
	setflag FLAG_DECORATION_7
	setflag FLAG_DECORATION_8
	setflag FLAG_DECORATION_9
	setflag FLAG_DECORATION_10
	setflag FLAG_DECORATION_11
	setflag FLAG_DECORATION_12
	setflag FLAG_DECORATION_13
	setflag FLAG_DECORATION_14
	setflag FLAG_DECORATION_15
	return

SecretBase_EventScript_InitDecorations:: @ 81A2F68
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, 0
	special InitSecretBaseDecorationSprites
	setvar VAR_SECRET_BASE_INITIALIZED, 1
	end

gUnknown_081A2F7B:: @ 81A2F7B
	setvar VAR_0x8005, 0
	goto EventScript_1A2F86
	end

EventScript_1A2F86:
	special SetDecoration
	end

gUnknown_081A2F8A:: @ 81A2F8A
	setvar VAR_0x8004, 0
	goto EventScript_1A2F95
	end

EventScript_1A2F95:
	special PutAwayDecorationIteration
	compare VAR_RESULT, 1
	goto_if_eq EventScript_1A2FBF
	addvar VAR_0x8004, 1
	compare VAR_0x8005, 0
	goto_if_eq EventScript_1A2F95
	removeobject VAR_0x8006
	setflag VAR_0x8005
	goto EventScript_1A2F95
	end

EventScript_1A2FBF:
	end

SecretBase_BlueCave1_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_BlueCave2_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_BlueCave3_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_BlueCave4_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_BrownCave1_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_BrownCave2_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_BrownCave3_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_BrownCave4_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_RedCave1_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_RedCave2_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_RedCave3_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_RedCave4_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Shrub1_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Shrub2_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Shrub3_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Shrub4_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Tree1_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Tree2_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Tree3_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_Tree4_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_YellowCave1_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_YellowCave2_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_YellowCave3_EventScript_1A2FC0:: @ 81A2FC0
SecretBase_YellowCave4_EventScript_1A2FC0:: @ 81A2FC0
	special GetSecretBaseOwnerAndState
	compare VAR_0x8004, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A3032
	compare VAR_0x8004, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A30AE
	compare VAR_0x8004, 2
	goto_if_eq SecretBase_RedCave1_EventScript_1A312A
	compare VAR_0x8004, 3
	goto_if_eq SecretBase_RedCave1_EventScript_1A31A6
	compare VAR_0x8004, 4
	goto_if_eq SecretBase_RedCave1_EventScript_1A3222
	compare VAR_0x8004, 5
	goto_if_eq SecretBase_RedCave1_EventScript_1A329E
	compare VAR_0x8004, 6
	goto_if_eq SecretBase_RedCave1_EventScript_1A331A
	compare VAR_0x8004, 7
	goto_if_eq SecretBase_RedCave1_EventScript_1A3396
	compare VAR_0x8004, 8
	goto_if_eq SecretBase_RedCave1_EventScript_1A3412
	compare VAR_0x8004, 9
	goto_if_eq SecretBase_RedCave1_EventScript_1A348E
	end

SecretBase_RedCave1_EventScript_1A3032:: @ 81A3032
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A3086
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A30A5
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer0Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A308F
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A308F
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer0AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A3086:: @ 81A3086
	msgbox SecretBase_Text_Trainer0PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A308F:: @ 81A308F
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer0DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A30A5:: @ 81A30A5
	msgbox SecretBase_Text_Trainer0PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A30AE:: @ 81A30AE
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A3102
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A3121
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer1Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A310B
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A310B
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer1AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A3102:: @ 81A3102
	msgbox SecretBase_Text_Trainer1PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A310B:: @ 81A310B
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer1DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3121:: @ 81A3121
	msgbox SecretBase_Text_Trainer1PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A312A:: @ 81A312A
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A317E
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A319D
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer2Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A3187
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A3187
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer2AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A317E:: @ 81A317E
	msgbox SecretBase_Text_Trainer2PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3187:: @ 81A3187
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer2DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A319D:: @ 81A319D
	msgbox SecretBase_Text_Trainer2PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A31A6:: @ 81A31A6
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A31FA
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A3219
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer3Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A3203
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A3203
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer3AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A31FA:: @ 81A31FA
	msgbox SecretBase_Text_Trainer3PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3203:: @ 81A3203
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer3DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3219:: @ 81A3219
	msgbox SecretBase_Text_Trainer3PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3222:: @ 81A3222
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A3276
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A3295
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer4Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A327F
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A327F
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer4AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A3276:: @ 81A3276
	msgbox SecretBase_Text_Trainer4PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A327F:: @ 81A327F
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer4DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3295:: @ 81A3295
	msgbox SecretBase_Text_Trainer4PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A329E:: @ 81A329E
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A32F2
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A3311
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer5Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A32FB
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A32FB
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer5AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A32F2:: @ 81A32F2
	msgbox SecretBase_Text_Trainer5PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A32FB:: @ 81A32FB
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer5DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3311:: @ 81A3311
	msgbox SecretBase_Text_Trainer5PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A331A:: @ 81A331A
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A336E
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A338D
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer6Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A3377
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A3377
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer6AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A336E:: @ 81A336E
	msgbox SecretBase_Text_Trainer6PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3377:: @ 81A3377
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer6DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A338D:: @ 81A338D
	msgbox SecretBase_Text_Trainer6PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3396:: @ 81A3396
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A33EA
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A3409
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer7Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A33F3
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A33F3
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer7AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A33EA:: @ 81A33EA
	msgbox SecretBase_Text_Trainer7PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A33F3:: @ 81A33F3
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer7DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3409:: @ 81A3409
	msgbox SecretBase_Text_Trainer7PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3412:: @ 81A3412
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A3466
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A3485
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer8Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A346F
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A346F
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer8AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A3466:: @ 81A3466
	msgbox SecretBase_Text_Trainer8PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A346F:: @ 81A346F
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer8DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3485:: @ 81A3485
	msgbox SecretBase_Text_Trainer8PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A348E:: @ 81A348E
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_RedCave1_EventScript_1A34E2
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_RedCave1_EventScript_1A3501
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer9Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_RedCave1_EventScript_1A34EB
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_RedCave1_EventScript_1A34EB
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer9AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_RedCave1_EventScript_1A350A
	end

SecretBase_RedCave1_EventScript_1A34E2:: @ 81A34E2
	msgbox SecretBase_Text_Trainer9PreChampion, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A34EB:: @ 81A34EB
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer9DeclineBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A3501:: @ 81A3501
	msgbox SecretBase_Text_Trainer9PostBattle, MSGBOX_NPC
	end

SecretBase_RedCave1_EventScript_1A350A:: @ 81A350A
	special sub_80BCE1C
	setvar VAR_0x8004, 1
	setvar VAR_0x8005, 0
	special DoSpecialTrainerBattle
	waitstate
	special HealPlayerParty
	release
	end
