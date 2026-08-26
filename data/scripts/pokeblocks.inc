.set NUM_OPPONENTS, VAR_0x8009

FallarborTown_ContestLobby_EventScript_BerryBlender1:: @ 81B7604
	setvar VAR_0x8008, 4
	setvar NUM_OPPONENTS, 1
	applymovement VAR_0x8008, BerryBlender_Movement_BlendLeaderWalkInPlace
	waitmovement 0
	goto BerryBlender_EventScript_BerryBlender1
	end

SlateportCity_ContestLobby_EventScript_BerryBlender2:: @ 81B761E
	setvar VAR_0x8008, 2
	setvar NUM_OPPONENTS, 2
	applymovement 6, Common_Movement_FaceOriginalDirection
	applymovement VAR_0x8008, BerryBlender_Movement_BlendLeaderWalkInPlace
	waitmovement 0
	goto BerryBlender_EventScript_BerryBlender1
	end

VerdanturfTown_ContestLobby_EventScript_BerryBlender1:: @ 81B763F
	setvar VAR_0x8008, 2
	setvar NUM_OPPONENTS, 1
	applymovement VAR_0x8008, BerryBlender_Movement_BlendLeaderWalkInPlace
	waitmovement 0
	goto BerryBlender_EventScript_BerryBlender1
	end

LilycoveCity_ContestLobby_EventScript_BerryBlender3:: @ 81B7659
	setvar VAR_0x8008, 3
	setvar NUM_OPPONENTS, 3
	applymovement 9, Common_Movement_FaceOriginalDirection
	applymovement 10, Common_Movement_FaceOriginalDirection
	applymovement VAR_0x8008, BerryBlender_Movement_BlendLeaderWalkInPlace
	waitmovement 0
	goto BerryBlender_EventScript_BerryBlender1
	end

BerryBlender_EventScript_BerryBlender1:: @ 81B7681
	lockall
	msgbox BerryBlender_Text_WantToMakePokeblocks, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq BerryBlender_EventScript_TryUseBerryBlender1
	compare VAR_RESULT, NO
	goto_if_eq BerryBlender_EventScript_DeclineBlender1
	end

BerryBlender_EventScript_TryUseBerryBlender1:: @ 81B76A1
	checkitem ITEM_POKEBLOCK_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq BerryBlender_EventScript_Blender1NoCase
	specialvar VAR_RESULT, PlayerHasBerries
	compare VAR_RESULT, 0
	goto_if_eq BerryBlender_EventScript_Blender1NoBerries
	msgbox BerryBlender_Text_Excellent, MSGBOX_DEFAULT
	specialvar VAR_RESULT, GetFirstFreePokeblockSlot
	compare VAR_RESULT, -1
	goto_if_ne BerryBlender_EventScript_UseBerryBlender1
	compare VAR_RESULT, -1
	goto_if_eq BerryBlender_EventScript_Blender1CaseFull
	end

BerryBlender_EventScript_DeclineBlender1:: @ 81B76E5
	msgbox BerryBlender_Text_MadeOldTimerSad, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_UseBerryBlender1:: @ 81B76EF
	msgbox BerryBlender_Text_KnowHowToMakePokeblocks, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq BerryBlender_EventScript_StartBlender1
	compare VAR_RESULT, NO
	goto_if_eq BerryBlender_EventScript_ExplainBlending1
	end

BerryBlender_EventScript_Blender1NoBerries:: @ 81B770E
	msgbox BerryBlender_Text_DontHaveAnyBerries, MSGBOX_DEFAULT
	dotimebasedevents
	goto_if_set FLAG_DAILY_RECEIVED_BERRY_FALLARBOR, BerryBlender_EventScript_Blender1NoSpareBerries
	goto BerryBlender_EventScript_Blender1GiveSpareBerry
	end

BerryBlender_EventScript_ExplainBlending1:: @ 81B7726
	msgbox BerryBlender_Text_ExplainBerryBlending, MSGBOX_DEFAULT
	goto BerryBlender_EventScript_StartBlender1
	end

BerryBlender_EventScript_StartBlender1:: @ 81B7734
	msgbox BerryBlender_Text_LetsBerryBlender, MSGBOX_DEFAULT
	goto BerryBlender_EventScript_DoBerryBlending
	end

BerryBlender_EventScript_DoBerryBlending:: @ 81B7742
	copyvar VAR_0x8004, NUM_OPPONENTS
	fadescreen FADE_TO_BLACK
	special DoBerryBlending
	waitstate
	releaseall
	end

BerryBlender_EventScript_Blender1NoSpareBerries:: @ 81B774F
	msgbox BerryBlender_Text_DontHaveAnyBerriesToSpare, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_Blender1GiveSpareBerry:: @ 81B7759
	msgbox BerryBlender_Text_CanHaveOneOfMyBerries, MSGBOX_DEFAULT
	giveitem ITEM_PECHA_BERRY
	setflag FLAG_DAILY_RECEIVED_BERRY_FALLARBOR
	goto BerryBlender_EventScript_UseBerryBlender1
	end

BerryBlender_EventScript_Blender1CaseFull:: @ 81B7776
	msgbox BerryBlender_Text_PokeblockCaseIsFull, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_Blender1NoCase:: @ 81B7780
	msgbox BerryBlender_Text_DontHavePokeblockCase, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_ContestLobby_EventScript_BerryBlenderOldMan:: @ 81B778A
	setvar VAR_0x8008, 4
	goto BerryBlender_EventScript_OldManCheckGiveBerry
	end

SlateportCity_ContestLobby_EventScript_BerryBlenderOldMan:: @ 81B7795
	setvar VAR_0x8008, 2
	goto BerryBlender_EventScript_OldManCheckGiveBerry
	end

VerdanturfTown_ContestLobby_EventScript_BerryBlenderOldMan:: @ 81B77A0
	setvar VAR_0x8008, 2
	goto BerryBlender_EventScript_OldManCheckGiveBerry
	end

LilycoveCity_ContestLobby_EventScript_BerryBlenderOldMan:: @ 81B77AB
	setvar VAR_0x8008, 3
	goto BerryBlender_EventScript_OldManCheckGiveBerry
	end

BerryBlender_EventScript_OldManCheckGiveBerry:: @ 81B77B6
	lock
	faceplayer
	msgbox BerryBlender_Text_LoveMakingPokeblocks, MSGBOX_DEFAULT
	specialvar VAR_RESULT, PlayerHasBerries
	compare VAR_RESULT, 1
	goto_if_eq BerryBlender_EventScript_OldManPlayerHasBerries
	compare VAR_RESULT, 0
	goto_if_eq BerryBlender_EventScript_OldManNoBerries
	end

BerryBlender_EventScript_OldManPlayerHasBerries:: @ 81B77DC
	msgbox BerryBlender_Text_MakePokeblocksUsingBerryBlender, MSGBOX_DEFAULT
	release
	end

BerryBlender_EventScript_OldManNoBerries:: @ 81B77E6
	dotimebasedevents
	goto_if_set FLAG_DAILY_RECEIVED_BERRY_FALLARBOR, BerryBlender_EventScript_OldManNoSpareBerries
	goto BerryBlender_EventScript_OldManGiveBerry
	end

BerryBlender_EventScript_OldManNoSpareBerries:: @ 81B77F6
	msgbox BerryBlender_Text_DontHaveAnyBerriesNoneToSpare, MSGBOX_DEFAULT
	release
	end

BerryBlender_EventScript_OldManGiveBerry:: @ 81B7800
	msgbox BerryBlender_Text_DontHaveAnyBerriesHaveOne, MSGBOX_DEFAULT
	giveitem ITEM_PECHA_BERRY
	setflag FLAG_DAILY_RECEIVED_BERRY_FALLARBOR
	msgbox BerryBlender_Text_UseItToMakePokeblocksTogether, MSGBOX_DEFAULT
	release
	end

BerryBlender_Movement_BlendLeaderWalkInPlace:: @ 81B7821
	walk_in_place_fastest_right
	step_end

FallarborTown_ContestLobby_EventScript_BerryBlenderLink:: @ 81B7823
	goto BerryBlender_EventScript_BerryBlenderLink
	end

SlateportCity_ContestLobby_EventScript_BerryBlenderLink:: @ 81B7829
	goto BerryBlender_EventScript_BerryBlenderLink
	end

VerdanturfTown_ContestLobby_EventScript_BerryBlenderLink:: @ 81B782F
	goto BerryBlender_EventScript_BerryBlenderLink
	end

LilycoveCity_ContestLobby_EventScript_BerryBlenderLink:: @ 81B7835
	goto BerryBlender_EventScript_BerryBlenderLink
	end

BerryBlender_EventScript_BerryBlenderLink:: @ 81B783B
	lockall
	specialvar VAR_RESULT, PlayerHasBerries
	compare VAR_RESULT, 0
	goto_if_eq BerryBlender_EventScript_LinkBlenderNoBerries
	checkitem ITEM_POKEBLOCK_CASE, 1
	compare VAR_RESULT, 0
	goto_if_eq BerryBlender_EventScript_LinkBlenderNoCase
	specialvar VAR_RESULT, GetFirstFreePokeblockSlot
	compare VAR_RESULT, -1
	goto_if_ne BerryBlender_EventScript_LinkBlenderSaveGame
	compare VAR_RESULT, -1
	goto_if_eq BerryBlender_EventScript_LinkBlenderCaseFull
	end

BerryBlender_EventScript_LinkBlenderSaveGame:: @ 81B7878
	msgbox BerryBlender_Text_SaveGameBeforeBerryBlenderLink, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq BerryBlender_EventScript_TryDoLinkBlender
	compare VAR_RESULT, NO
	goto_if_eq BerryBlender_EventScript_CancelLinkBlender
	end

BerryBlender_EventScript_LinkBlenderNoBerries:: @ 81B7897
	msgbox BerryBlender_Text_NoBerriesLink, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_TryDoLinkBlender:: @ 81B78A1
	call Common_EventScript_SaveGame
	compare VAR_RESULT, 0
	goto_if_eq BerryBlender_EventScript_CancelLinkBlender
	message BerryBlender_Text_SearchingForFriends
	waitmessage
	special sub_8083614
	waitstate
	compare VAR_RESULT, 1
	goto_if_eq BerryBlender_EventScript_SpawnLinkPartners
	compare VAR_RESULT, 2
	goto_if_eq BerryBlender_EventScript_CloseLinkNotReady
	compare VAR_RESULT, 3
	goto_if_eq BerryBlender_EventScript_CloseLinkDifferentSelections
	compare VAR_RESULT, 5
	goto_if_eq BerryBlender_EventScript_CloseLink
	compare VAR_RESULT, 6
	goto_if_eq BerryBlender_EventScript_LinkError
	end

BerryBlender_EventScript_TwoPlayerLink:: @ 81B78F3
	msgbox BerryBlender_Text_Player1Arrived, MSGBOX_DEFAULT
	goto BerryBlender_EventScript_DoLinkBerryBlending
	end

BerryBlender_EventScript_ThreePlayerLink:: @ 81B7901
	msgbox BerryBlender_Text_Player1And2Arrived, MSGBOX_DEFAULT
	goto BerryBlender_EventScript_DoLinkBerryBlending
	end

BerryBlender_EventScript_FourPlayerLink:: @ 81B790F
	msgbox BerryBlender_Text_AllPlayersArrived, MSGBOX_DEFAULT
	goto BerryBlender_EventScript_DoLinkBerryBlending
	end

BerryBlender_EventScript_DoLinkBerryBlending:: @ 81B791D
	setvar VAR_0x8004, 0
	fadescreen FADE_TO_BLACK
	removeobject 240
	removeobject 239
	removeobject 238
	removeobject 237
	special DoBerryBlending
	waitstate
	releaseall
	end

BerryBlender_EventScript_CancelLinkBlender:: @ 81B7936
	releaseall
	end

BerryBlender_EventScript_LinkBlenderCaseFull:: @ 81B7938
	msgbox BerryBlender_Text_PokeblockCaseIsFullLink, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_LinkBlenderNoCase:: @ 81B7942
	msgbox BerryBlender_Text_DontHavePokeblockCaseLink, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_CloseLinkNotReady:: @ 81B794C
	special CloseLink
	msgbox Text_SomeoneIsNotReadyToLink, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_CloseLinkDifferentSelections:: @ 81B7959
	special CloseLink
	msgbox Text_PlayersMadeDifferentSelections, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_CloseLink:: @ 81B7966
	special CloseLink
	msgbox gText_PokeblockLinkCanceled, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_LinkError:: @ 81B7973
	special CloseLink
	msgbox Text_LinkErrorPleaseReset, MSGBOX_DEFAULT
	releaseall
	end

BerryBlender_EventScript_SpawnLinkPartners:: @ 81B7980
	fadescreen FADE_TO_BLACK
	specialvar VAR_RESULT, GetLinkPartnerNames
	copyvar VAR_0x8008, VAR_RESULT
	copyvar VAR_0x8004, VAR_0x8008
	special SpawnBerryBlenderLinkPlayerSprites
	goto BerryBlender_EventScript_LinkPlayersArrived
	end

BerryBlender_EventScript_LinkPlayersArrived:: @ 81B799A
	fadescreen FADE_FROM_BLACK
	switch VAR_0x8008
	case 2, BerryBlender_EventScript_TwoPlayerLink
	case 3, BerryBlender_EventScript_ThreePlayerLink
	case 4, BerryBlender_EventScript_FourPlayerLink
	end
