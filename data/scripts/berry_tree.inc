BerryTreeScript:: @ 81A14DD
	special ObjectEventInteractionGetBerryTreeData
	switch VAR_0x8004
	case 255, BerryTree_EventScript_Sparkling
	case 0, BerryTree_EventScript_CheckSoil
	case 1, BerryTree_EventScript_CheckBerryStage1
	case 2, BerryTree_EventScript_CheckBerryStage2
	case 3, BerryTree_EventScript_CheckBerryStage3
	case 4, BerryTree_EventScript_CheckBerryStage4
	case 5, BerryTree_EventScript_CheckBerryFullyGrown
	end

BerryTree_EventScript_Sparkling:: @ 81A1533
	lockall
	message BerryTree_Text_ExclamationPoint
	waitmessage
	waitbuttonpress
	releaseall
	end

BerryTree_EventScript_CheckSoil:: @ 81A153D
	lock
	faceplayer
	specialvar VAR_RESULT, PlayerHasBerries
	compare VAR_RESULT, 1
	goto_if_eq BerryTree_EventScript_WantToPlant
	message BerryTree_Text_ItsSoftLoamySoil
	waitmessage
	waitbuttonpress
	release
	end

BerryTree_EventScript_WantToPlant:: @ 81A1558
	msgbox BerryTree_Text_WantToPlant, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq BerryTree_EventScript_ChooseBerryToPlant
	compare VAR_RESULT, NO
	goto_if_eq BerryTree_EventScript_CancelPlanting
	end

BerryTree_EventScript_ChooseBerryToPlant:: @ 81A1577
	fadescreen FADE_TO_BLACK
	closemessage
	special Berry_FadeAndGoToBerryBagMenu
	waitstate
	compare VAR_ITEM_ID, 0
	goto_if_eq BerryTree_EventScript_CancelPlanting
	removeitem VAR_ITEM_ID, 1
	call BerryTree_EventScript_PlantBerry
BerryTree_EventScript_CancelPlanting:: @ 81A1593
	release
	end

BerryTree_EventScript_CheckBerryStage1:: @ 81A1595
	lockall
	message BerryTree_Text_BerryGrowthStage1
	waitmessage
	waitbuttonpress
	goto BerryTree_EventScript_WantToWater

BerryTree_EventScript_CheckBerryStage2:: @ 81A15A2
	lockall
	message BerryTree_Text_BerryGrowthStage2
	waitmessage
	waitbuttonpress
	goto BerryTree_EventScript_WantToWater

BerryTree_EventScript_CheckBerryStage3:: @ 81A15AF
	lockall
	message BerryTree_Text_BerryGrowthStage3
	waitmessage
	waitbuttonpress
	goto BerryTree_EventScript_WantToWater

BerryTree_EventScript_CheckBerryStage4:: @ 81A15BC
	call BerryTree_EventScript_GetCareAdverb
	lockall
	message BerryTree_Text_BerryGrowthStage4
	waitmessage
	waitbuttonpress
	goto BerryTree_EventScript_WantToWater

BerryTree_EventScript_GetCareAdverb:: @ 81A15CE
	compare VAR_0x8005, 0
	goto_if_eq BerryTree_EventScript_SetAdverbPoor
	compare VAR_0x8005, 4
	goto_if_eq BerryTree_EventScript_SetAdverbGreat
	bufferstring 1, BerryTree_Text_CareAdverbGood
	return

BerryTree_EventScript_SetAdverbGreat:: @ 81A15EB
	bufferstring 1, BerryTree_Text_CareAdverbGreat
	return

BerryTree_EventScript_SetAdverbPoor:: @ 81A15F2
	bufferstring 1, BerryTree_Text_CareAdverbPoor
	return

BerryTree_EventScript_CheckBerryFullyGrown:: @ 81A15F9
	buffernumberstring 1, VAR_0x8006
	lock
	faceplayer
	msgbox BerryTree_Text_WantToPick, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq BerryTree_EventScript_PickBerry
	compare VAR_RESULT, NO
	goto_if_eq BerryTree_EventScript_CancelPickingBerry

BerryTree_EventScript_PickBerry:: @ 81A161D
	special ObjectEventInteractionPickBerryTree
	compare VAR_0x8004, 0
	goto_if_eq BerryTree_EventScript_BerryPocketFull
	special ObjectEventInteractionRemoveBerryTree
	message BerryTree_Text_PickedTheBerry
	playfanfare MUS_OBTAIN_BERRY
	waitmessage
	waitfanfare
	waitbuttonpress
	message BerryTree_Text_PutAwayBerry
	waitmessage
	waitbuttonpress
	release
	end

BerryTree_EventScript_BerryPocketFull:: @ 81A1642
	message BerryTree_Text_BerryPocketFull
	waitmessage
	waitbuttonpress
	release
	end

BerryTree_EventScript_CancelPickingBerry:: @ 81A164B
	message BerryTree_Text_BerryLeftUnpicked
	waitmessage
	waitbuttonpress
	release
	end

BerryTree_EventScript_ItemUsePlantBerry:: @ 81A1654
	lockall
	special ObjectEventInteractionGetBerryTreeData
	call BerryTree_EventScript_PlantBerry
	releaseall
	end

BerryTree_EventScript_WantToWater:: @ 81A165F
	checkitem ITEM_WAILMER_PAIL, 1
	compare VAR_RESULT, 0
	goto_if_eq BerryTree_EventScript_DontWater
	msgbox BerryTree_Text_WantToWater, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq BerryTree_EventScript_WaterBerry
	compare VAR_RESULT, NO
	goto_if_eq BerryTree_EventScript_DontWater
BerryTree_EventScript_DontWater:: @ 81A168D
	releaseall
	end

BerryTree_EventScript_ItemUseWailmerPail:: @ 81A168F
	special ObjectEventInteractionGetBerryTreeData
	lockall
BerryTree_EventScript_WaterBerry:: @ 81A1693
	message BerryTree_Text_WateredTheBerry
	waitmessage
	special ObjectEventInteractionWaterBerryTree
	special DoWateringBerryTreeAnim
	waitstate
	message BerryTree_Text_PlantIsDelighted
	waitmessage
	waitbuttonpress
	releaseall
	end

BerryTree_EventScript_PlantBerry:: @ 81A16A9
	special ObjectEventInteractionPlantBerryTree
	incrementgamestat GAME_STAT_PLANTED_BERRIES
	message BerryTree_Text_PlantedOneBerry
	waitmessage
	waitbuttonpress
	return
