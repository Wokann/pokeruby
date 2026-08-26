SecretBase_EventScript_CheckEntrance:: @ 81A2C51
	special GetSecretBaseTypeInFrontOfPlayer
	special CheckPlayerHasSecretBase
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_AlreadyHasSecretBase
	checkpartymove MOVE_SECRET_POWER
	setfieldeffectargument 0, VAR_RESULT
	buffermovename 1, MOVE_SECRET_POWER
	compare VAR_0x8007, 1
	goto_if_eq SecretBase_EventScript_Cave
	compare VAR_0x8007, 2
	goto_if_eq SecretBase_EventScript_Cave
	compare VAR_0x8007, 3
	goto_if_eq SecretBase_EventScript_Cave
	compare VAR_0x8007, 4
	goto_if_eq SecretBase_EventScript_Cave
	compare VAR_0x8007, 5
	goto_if_eq SecretBase_EventScript_Tree
	compare VAR_0x8007, 6
	goto_if_eq SecretBase_EventScript_Shrub
	end

SecretBase_EventScript_Cave:
	lockall
	compare VAR_RESULT, 6
	goto_if_eq SecretBase_EventScript_CaveNoSecretPower
	bufferpartymonnick 0, VAR_RESULT
	msgbox SecretBase_Text_IndentUseSecretPower, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_CancelOnEntrance
	msgbox UsedCutRockSmashText, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_SECRET_POWER_CAVE
	waitstate
	goto SecretBase_EventScript_CaveEnter
	end

SecretBase_EventScript_CaveUseSecretPower:: @ 81A2CE6
	lockall
	dofieldeffect FLDEFF_USE_SECRET_POWER_CAVE
	waitstate
	goto SecretBase_EventScript_CaveEnter
	end

SecretBase_EventScript_CaveNoSecretPower:
	msgbox SecretBase_Text_SmallIndentInWall, MSGBOX_SIGN
	end

SecretBase_EventScript_CaveEnter:
	msgbox SecretBase_Text_DiscoveredSmallCavern, MSGBOX_DEFAULT
	goto SecretBase_EventScript_InitSecretBase
	end

SecretBase_EventScript_Tree:
	lockall
	compare VAR_RESULT, 6
	goto_if_eq SecretBase_EventScript_TreeNoSecretPower
	bufferpartymonnick 0, VAR_RESULT
	msgbox SecretBase_Text_TreeUseSecretPower, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_CancelOnEntrance
	msgbox UsedCutRockSmashText, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_SECRET_POWER_TREE
	waitstate
	goto SecretBase_EventScript_TreeEnter
	end

SecretBase_EventScript_TreeUseSecretPower:: @ 81A2D3E
	lockall
	dofieldeffect FLDEFF_USE_SECRET_POWER_TREE
	waitstate
	goto SecretBase_EventScript_TreeEnter
	end

SecretBase_EventScript_TreeNoSecretPower:
	msgbox SecretBase_Text_TreeCanBeClimbed, MSGBOX_SIGN
	end

SecretBase_EventScript_TreeEnter:
	msgbox SecretBase_Text_VineDroppedDown, MSGBOX_DEFAULT
	goto SecretBase_EventScript_InitSecretBase
	end

SecretBase_EventScript_Shrub:
	lockall
	compare VAR_RESULT, 6
	goto_if_eq SecretBase_EventScript_ShrubNoSecretPower
	bufferpartymonnick 0, VAR_RESULT
	msgbox SecretBase_Text_ClumpUseSecretPower, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_CancelOnEntrance
	msgbox UsedCutRockSmashText, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_USE_SECRET_POWER_SHRUB
	waitstate
	goto SecretBase_EventScript_ShrubEnter
	end

SecretBase_EventScript_ShrubUseSecretPower:: @ 81A2D96
	lockall
	dofieldeffect FLDEFF_USE_SECRET_POWER_SHRUB
	waitstate
	goto SecretBase_EventScript_ShrubEnter
	end

SecretBase_EventScript_ShrubNoSecretPower:
	msgbox SecretBase_Text_ClumpOfGrass, MSGBOX_SIGN
	end

SecretBase_EventScript_ShrubEnter:
	msgbox SecretBase_Text_DiscoveredSmallEntrance, MSGBOX_DEFAULT
	goto SecretBase_EventScript_InitSecretBase
	end

SecretBase_EventScript_InitSecretBase:
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
	applymovement LOCALID_PLAYER, SecretBase_Movement_EnterBase
	waitmovement 0
	setvar VAR_INIT_SECRET_BASE, 1
	msgbox SecretBase_Text_WantToMakeYourSecretBaseHere, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq SecretBase_EventScript_SetAsBase
	closemessage
	playse SE_EXIT
	special ClearAndLeaveSecretBase
	end

SecretBase_EventScript_SetAsBase:: @ 81A2E08
	closemessage
	setflag FLAG_RECEIVED_SECRET_POWER
	special EnterNewlyCreatedSecretBase
	waitstate
	end

SecretBase_Movement_EnterBase:: @ 81A2E11
	walk_up
	walk_up
	step_end

SecretBase_EventScript_Enter:: @ 81A2E14
	lockall
	setvar VAR_INIT_SECRET_BASE, 1
	playse SE_EXIT
	special IsCurSecretBaseOwnedByAnotherPlayer
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_EnterPlayersBase
	clearflag FLAG_DECORATION_1
	special EnterSecretBase
	setvar VAR_SECRET_BASE_INITIALIZED, 0
	waitstate
	end

SecretBase_EventScript_EnterPlayersBase:
	setflag FLAG_DECORATION_1
	special EnterSecretBase
	setvar VAR_SECRET_BASE_INITIALIZED, 0
	waitstate
	end

SecretBase_EventScript_AlreadyHasSecretBase:
	checkpartymove MOVE_SECRET_POWER
	compare VAR_RESULT, 6
	goto_if_eq SecretBase_EventScript_NoSecretPower
	setfieldeffectargument 0, VAR_RESULT
	setorcopyvar VAR_0x8004, VAR_RESULT
	lockall
	special GetSecretBaseNearbyMapName
	msgbox SecretBase_Text_WouldYouLikeToMoveBases, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_CancelOnEntrance
	msgbox SecretBase_Text_AllDecorationsWillBeReturned, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_CancelOnEntrance
	fadescreen FADE_TO_BLACK
	special MoveSecretBase
	closemessage
	fadescreen FADE_FROM_BLACK
	msgbox SecretBase_Text_MovingCompletedUseSecretPower, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_CancelOnEntrance
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

SecretBase_EventScript_NoSecretPower::
	compare VAR_0x8007, 1
	goto_if_eq SecretBase_EventScript_CaveNoSecretPower
	compare VAR_0x8007, 2
	goto_if_eq SecretBase_EventScript_CaveNoSecretPower
	compare VAR_0x8007, 3
	goto_if_eq SecretBase_EventScript_CaveNoSecretPower
	compare VAR_0x8007, 4
	goto_if_eq SecretBase_EventScript_CaveNoSecretPower
	compare VAR_0x8007, 5
	goto_if_eq SecretBase_EventScript_TreeNoSecretPower
	compare VAR_0x8007, 6
	goto_if_eq SecretBase_EventScript_ShrubNoSecretPower
	end

SecretBase_EventScript_CancelOnEntrance::
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

SecretBase_EventScript_SetDecoration:: @ 81A2F7B
	setvar VAR_0x8005, 0
	goto SecretBase_EventScript_SetDecoration2
	end

SecretBase_EventScript_SetDecoration2:
	special SetDecoration
	end

SecretBase_EventScript_PutAwayDecoration:: @ 81A2F8A
	setvar VAR_0x8004, 0
	goto SecretBase_EventScript_PutAwayDecorationLoop
	end

SecretBase_EventScript_PutAwayDecorationLoop:
	special PutAwayDecorationIteration
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_PutAwayDecorationEnd
	addvar VAR_0x8004, 1
	compare VAR_0x8005, 0
	goto_if_eq SecretBase_EventScript_PutAwayDecorationLoop
	removeobject VAR_0x8006
	setflag VAR_0x8005
	goto SecretBase_EventScript_PutAwayDecorationLoop
	end

SecretBase_EventScript_PutAwayDecorationEnd:
	end

SecretBase_EventScript_RecordMixTrainer:: @ 81A2FC0
	special GetSecretBaseOwnerAndState
	compare VAR_0x8004, 0
	goto_if_eq SecretBase_EventScript_Trainer0
	compare VAR_0x8004, 1
	goto_if_eq SecretBase_EventScript_Trainer1
	compare VAR_0x8004, 2
	goto_if_eq SecretBase_EventScript_Trainer2
	compare VAR_0x8004, 3
	goto_if_eq SecretBase_EventScript_Trainer3
	compare VAR_0x8004, 4
	goto_if_eq SecretBase_EventScript_Trainer4
	compare VAR_0x8004, 5
	goto_if_eq SecretBase_EventScript_Trainer5
	compare VAR_0x8004, 6
	goto_if_eq SecretBase_EventScript_Trainer6
	compare VAR_0x8004, 7
	goto_if_eq SecretBase_EventScript_Trainer7
	compare VAR_0x8004, 8
	goto_if_eq SecretBase_EventScript_Trainer8
	compare VAR_0x8004, 9
	goto_if_eq SecretBase_EventScript_Trainer9
	end

SecretBase_EventScript_Trainer0:: @ 81A3032
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer0PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer0PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer0Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer0DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer0DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer0AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer0PreChampion:: @ 81A3086
	msgbox SecretBase_Text_Trainer0PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer0DeclineBattle:: @ 81A308F
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer0DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer0PostBattle:: @ 81A30A5
	msgbox SecretBase_Text_Trainer0PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer1:: @ 81A30AE
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer1PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer1PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer1Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer1DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer1DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer1AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer1PreChampion:: @ 81A3102
	msgbox SecretBase_Text_Trainer1PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer1DeclineBattle:: @ 81A310B
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer1DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer1PostBattle:: @ 81A3121
	msgbox SecretBase_Text_Trainer1PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer2:: @ 81A312A
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer2PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer2PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer2Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer2DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer2DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer2AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer2PreChampion:: @ 81A317E
	msgbox SecretBase_Text_Trainer2PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer2DeclineBattle:: @ 81A3187
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer2DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer2PostBattle:: @ 81A319D
	msgbox SecretBase_Text_Trainer2PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer3:: @ 81A31A6
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer3PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer3PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer3Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer3DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer3DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer3AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer3PreChampion:: @ 81A31FA
	msgbox SecretBase_Text_Trainer3PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer3DeclineBattle:: @ 81A3203
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer3DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer3PostBattle:: @ 81A3219
	msgbox SecretBase_Text_Trainer3PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer4:: @ 81A3222
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer4PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer4PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer4Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer4DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer4DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer4AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer4PreChampion:: @ 81A3276
	msgbox SecretBase_Text_Trainer4PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer4DeclineBattle:: @ 81A327F
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer4DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer4PostBattle:: @ 81A3295
	msgbox SecretBase_Text_Trainer4PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer5:: @ 81A329E
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer5PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer5PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer5Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer5DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer5DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer5AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer5PreChampion:: @ 81A32F2
	msgbox SecretBase_Text_Trainer5PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer5DeclineBattle:: @ 81A32FB
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer5DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer5PostBattle:: @ 81A3311
	msgbox SecretBase_Text_Trainer5PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer6:: @ 81A331A
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer6PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer6PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer6Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer6DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer6DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer6AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer6PreChampion:: @ 81A336E
	msgbox SecretBase_Text_Trainer6PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer6DeclineBattle:: @ 81A3377
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer6DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer6PostBattle:: @ 81A338D
	msgbox SecretBase_Text_Trainer6PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer7:: @ 81A3396
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer7PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer7PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer7Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer7DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer7DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer7AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer7PreChampion:: @ 81A33EA
	msgbox SecretBase_Text_Trainer7PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer7DeclineBattle:: @ 81A33F3
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer7DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer7PostBattle:: @ 81A3409
	msgbox SecretBase_Text_Trainer7PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer8:: @ 81A3412
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer8PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer8PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer8Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer8DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer8DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer8AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer8PreChampion:: @ 81A3466
	msgbox SecretBase_Text_Trainer8PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer8DeclineBattle:: @ 81A346F
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer8DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer8PostBattle:: @ 81A3485
	msgbox SecretBase_Text_Trainer8PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer9:: @ 81A348E
	goto_if_unset FLAG_SYS_GAME_CLEAR, SecretBase_EventScript_Trainer9PreChampion
	compare VAR_RESULT, 1
	goto_if_eq SecretBase_EventScript_Trainer9PostBattle
	lock
	faceplayer
	msgbox SecretBase_Text_Trainer9Intro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq SecretBase_EventScript_Trainer9DeclineBattle
	setvar VAR_RESULT, 1
	special SetBattledOwnerFromResult
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq SecretBase_EventScript_Trainer9DeclineBattle
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer9AcceptBattle, MSGBOX_DEFAULT
	goto SecretBase_EventScript_BattleTrainer
	end

SecretBase_EventScript_Trainer9PreChampion:: @ 81A34E2
	msgbox SecretBase_Text_Trainer9PreChampion, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer9DeclineBattle:: @ 81A34EB
	setvar VAR_RESULT, 0
	special SetBattledOwnerFromResult
	erasebox 0, 0, 15, 10
	msgbox SecretBase_Text_Trainer9DeclineBattle, MSGBOX_NPC
	end

SecretBase_EventScript_Trainer9PostBattle:: @ 81A3501
	msgbox SecretBase_Text_Trainer9PostBattle, MSGBOX_NPC
	end

SecretBase_EventScript_BattleTrainer:: @ 81A350A
	special PrepareSecretBaseTrainerBattle
	setvar VAR_0x8004, 1
	setvar VAR_0x8005, 0
	special StartSpecialBattle
	waitstate
	special ScrSpecial_HealPlayerParty
	release
	end
