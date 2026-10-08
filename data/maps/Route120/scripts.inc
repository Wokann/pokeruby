Route120_MapScripts:: @ 815160E
	map_script MAP_SCRIPT_ON_RESUME, Route120_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, Route120_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route120_OnLoad
	.byte 0

Route120_OnResume:: @ 815161E
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, Route120_EventScript_RemoveKecleonObject
	end

Route120_EventScript_RemoveKecleonObject:: @ 8151628
	compare VAR_0x8004, 0
	call_if_eq Route120_EventScript_RemoveBridgeKecleon
	compare VAR_0x8004, 1
	call_if_eq Route120_EventScript_151672
	compare VAR_0x8004, 2
	call_if_eq Route120_EventScript_151676
	compare VAR_0x8004, 3
	call_if_eq Route120_EventScript_15167A
	compare VAR_0x8004, 4
	call_if_eq Route120_EventScript_15167E
	compare VAR_0x8004, 5
	call_if_eq Route120_EventScript_151682
	return

Route120_EventScript_RemoveBridgeKecleon:: @ 815166B
	removeobject 30
	removeobject 36
	return

Route120_EventScript_151672:: @ 8151672
	removeobject 37
	return

Route120_EventScript_151676:: @ 8151676
	removeobject 38
	return

Route120_EventScript_15167A:: @ 815167A
	removeobject 39
	return

Route120_EventScript_15167E:: @ 815167E
	removeobject 41
	return

Route120_EventScript_151682:: @ 8151682
	removeobject 40
	return

Route120_OnLoad:: @ 8151686
	call_if_unset FLAG_REGI_DOORS_OPENED, Route120_EventScript_CloseAncientTomb
	call_if_set FLAG_RECEIVED_DEVON_SCOPE, Route120_EventScript_SetBridgeClearMetatiles
	call_if_unset FLAG_RECEIVED_DEVON_SCOPE, Route120_EventScript_SetBridgeKecleonMovement
	end

Route120_EventScript_CloseAncientTomb:: @ 81516A2
	setmetatile 7, 54, METATILE_General_RockWall_RockBase, 1
	setmetatile 7, 55, METATILE_General_RockWall_SandBase, 1
	return

Route120_EventScript_SetBridgeClearMetatiles:: @ 81516B5
	setmetatile 13, 15, METATILE_Fortree_WoodBridge1_Top, 0
	setmetatile 12, 16, METATILE_Fortree_WoodBridge1_Bottom, 0
	setmetatile 12, 17, METATILE_General_ReflectiveWater, 0
	setmetatile 13, 17, METATILE_General_ReflectiveWater, 0
	return

Route120_EventScript_SetBridgeKecleonMovement:: @ 81516DA
	setobjectmovementtype 36, 10
	return

Route120_OnTransition:: @ 81516DF
	call GabbyAndTy_EventScript_UpdateLocation
	call Route120_EventScript_SetWeather
	end

Route120_EventScript_SetWeather:: @ 81516EA
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	compare VAR_TEMP_1, 14
	goto_if_le Route120_EventScript_SetSunnyWeather
	compare VAR_TEMP_1, 60
	goto_if_le Route120_EventScript_TrySetRainyWeather
	compare VAR_TEMP_1, 61
	goto_if_ge Route120_EventScript_SetCloudyWeather
	return

Route120_EventScript_SetCloudyWeather:: @ 8151711
	setweather WEATHER_SUNNY_CLOUDS
	return

Route120_EventScript_SetSunnyWeather:: @ 8151715
	setweather WEATHER_SUNNY
	return

Route120_EventScript_TrySetRainyWeather:: @ 8151719
	compare VAR_TEMP_0, 7
	goto_if_le Route120_EventScript_SetRainyWeather
	compare VAR_TEMP_0, 19
	goto_if_le Route120_EventScript_SetSunnyWeather
	goto Route120_EventScript_SetRainyWeather
	end

Route120_EventScript_SetRainyWeather:: @ 8151735
	setweather WEATHER_RAIN
	return

Route120_EventScript_BerryBeauty:: @ 8151739
	lock
	faceplayer
	dotimebasedevents
	goto_if_set FLAG_DAILY_ROUTE_120_RECEIVED_BERRY, Route120_EventScript_ReceivedBerry
	msgbox Route120_Text_1C58F1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq Route120_EventScript_BerryLove
	compare VAR_RESULT, NO
	call_if_eq Route120_EventScript_BerryNotLove
	specialvar VAR_RESULT, GetPlayerTrainerIdOnesDigit
	switch VAR_RESULT
	case 0, Route120_EventScript_GiveFigyBerry
	case 5, Route120_EventScript_GiveFigyBerry
	case 1, Route120_EventScript_GiveWikiBerry
	case 6, Route120_EventScript_GiveWikiBerry
	case 2, Route120_EventScript_GiveMagoBerry
	case 7, Route120_EventScript_GiveMagoBerry
	case 3, Route120_EventScript_GiveAguavBerry
	case 8, Route120_EventScript_GiveAguavBerry
	case 4, Route120_EventScript_GiveIapapaBerry
	case 9, Route120_EventScript_GiveIapapaBerry
	end

Route120_EventScript_GiveFigyBerry:: @ 81517DC
	setvar VAR_0x8004, ITEM_FIGY_BERRY
	goto Route120_EventScript_GiveBerry
	end

Route120_EventScript_GiveWikiBerry:: @ 81517E7
	setvar VAR_0x8004, ITEM_WIKI_BERRY
	goto Route120_EventScript_GiveBerry
	end

Route120_EventScript_GiveMagoBerry:: @ 81517F2
	setvar VAR_0x8004, ITEM_MAGO_BERRY
	goto Route120_EventScript_GiveBerry
	end

Route120_EventScript_GiveAguavBerry:: @ 81517FD
	setvar VAR_0x8004, ITEM_AGUAV_BERRY
	goto Route120_EventScript_GiveBerry
	end

Route120_EventScript_GiveIapapaBerry:: @ 8151808
	setvar VAR_0x8004, ITEM_IAPAPA_BERRY
	goto Route120_EventScript_GiveBerry
	end

Route120_EventScript_GiveBerry:: @ 8151813
	giveitem VAR_0x8004
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_DAILY_ROUTE_120_RECEIVED_BERRY
	msgbox Route120_Text_BerryIsRareRaiseItWithCare, MSGBOX_DEFAULT
	release
	end

Route120_EventScript_ReceivedBerry:: @ 8151837
	msgbox Route120_Text_IllGetMoreBerriesFromBerryMaster, MSGBOX_DEFAULT
	release
	end

Route120_EventScript_BerryLove:: @ 8151841
	msgbox Route120_Text_YesYouUnderstand, MSGBOX_DEFAULT
	return

Route120_EventScript_BerryNotLove:: @ 815184A
	msgbox Route120_Text_MakeYourOwnImpressions, MSGBOX_DEFAULT
	return

Route120_EventScript_Steven:: @ 8151853
	lock
	faceplayer
	goto_if_set FLAG_NOT_READY_FOR_BATTLE_ROUTE_120, Route120_EventScript_StevenAskReadyForBattle
	msgbox Route120_Text_StevenGreeting, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route120_EventScript_StevenNotReady
	goto Route120_EventScript_StevenBattleKecleon
	end

Route120_EventScript_StevenNotReady:: @ 8151877
	msgbox Route120_Text_StevenIllWaitHere, MSGBOX_DEFAULT
	setflag FLAG_NOT_READY_FOR_BATTLE_ROUTE_120
	release
	end

Route120_EventScript_StevenAskReadyForBattle:: @ 8151884
	msgbox Route120_Text_StevenReadyForBattle, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route120_EventScript_StevenNotReady
	goto Route120_EventScript_StevenBattleKecleon
	end

Route120_EventScript_StevenBattleKecleon:: @ 815189D
	msgbox Route120_Text_StevenShowMeYourPower, MSGBOX_DEFAULT
	closemessage
	compare VAR_FACING, 2
	call_if_eq Route120_EventScript_PlayerApproachKecleonNorth
	compare VAR_FACING, 3
	call_if_eq Route120_EventScript_PlayerApproachKecleonWest
	applymovement 31, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	delay 20
	msgbox Route120_Text_StevenUsedDevonScope, MSGBOX_DEFAULT
	closemessage
	applymovement 30, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	applymovement 30, Movement_KecleonAppears
	waitmovement 0
	waitse
	playmoncry SPECIES_KECLEON, 2
	delay 40
	waitmoncry
	setwildbattle SPECIES_KECLEON, 30, ITEM_NONE
	setvar VAR_0x8004, 0
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	goto Route120_EventScript_StevenGiveDeconScope
	end

Route120_EventScript_StevenGiveDeconScope:: @ 8151908
	applymovement 31, Common_Movement_WalkInPlaceFasterDown
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox Route120_Text_StevenGiveDevonScope, MSGBOX_DEFAULT
	giveitem ITEM_DEVON_SCOPE
	setflag FLAG_RECEIVED_DEVON_SCOPE
	msgbox Route120_Text_StevenGoodbye, MSGBOX_DEFAULT
	closemessage
	applymovement 31, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 50
	setfieldeffectargument 0, 1
	dofieldeffect FLDEFF_NPCFLY_OUT
	delay 15
	removeobject 31
	waitfieldeffect FLDEFF_NPCFLY_OUT
	setmetatile 13, 15, METATILE_Fortree_WoodBridge1_Top, 0
	setmetatile 12, 16, METATILE_Fortree_WoodBridge1_Bottom, 0
	setmetatile 12, 17, METATILE_General_ReflectiveWater, 0
	setmetatile 13, 17, METATILE_General_ReflectiveWater, 0
	special DrawWholeMapView
	release
	end

Route120_EventScript_PlayerApproachKecleonNorth:: @ 815197F
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	return

Route120_EventScript_PlayerApproachKecleonWest:: @ 815198A
	applymovement LOCALID_PLAYER, Route120_Movement_ApproachKecleonWest
	waitmovement 0
	return

Route120_Movement_ApproachKecleonWest:: @ 8151995
	walk_down
	walk_left
	step_end

Route120_EventScript_BridgeKecleon:: @ 8151998
	msgbox Kecleon_Text_SomethingUnseeable, MSGBOX_NPC
	end

Route120_EventScript_RouteSignFortree:: @ 81519A1
	msgbox Route120_Text_RouteSignFortree, MSGBOX_SIGN
	end

Route120_EventScript_RouteSign121:: @ 81519AA
	msgbox Route120_Text_RouteSign121, MSGBOX_SIGN
	end

Route120_EventScript_Colin:: @ 81519B3
	trainerbattle_single TRAINER_COLIN, Route120_Text_ColinIntro, Route120_Text_ColinDefeat
	msgbox Route120_Text_ColinPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_Robert:: @ 81519CA
	trainerbattle_single TRAINER_ROBERT_1, Route120_Text_RobertIntro, Route120_Text_RobertDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route120_EventScript_RematchRobert
	msgbox Route120_Text_RobertPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_RematchRobert:: @ 81519F1
	trainerbattle_rematch TRAINER_ROBERT_1, Route120_Text_RobertRematchIntro, Route120_Text_RobertRematchDefeat
	msgbox Route120_Text_RobertPostRematch, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_151A08:: @ 8151A08
	trainerbattle_single TRAINER_CARLOS, Route120_Text_1BF193, Route120_Text_1BF1D4
	msgbox Route120_Text_1BF215, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_Jenna:: @ 8151A1F
	trainerbattle_single TRAINER_JENNA, Route120_Text_JennaIntro, Route120_Text_JennaDefeat
	msgbox Route120_Text_JennaPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_151A36:: @ 8151A36
	trainerbattle_single TRAINER_BRANDON_1, Route120_Text_1BF32B, Route120_Text_1BF36B
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route120_EventScript_151A5D
	msgbox Route120_Text_1BF38D, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_151A5D:: @ 8151A5D
	trainerbattle_rematch TRAINER_BRANDON_1, Route120_Text_1BF3D0, Route120_Text_1BF416
	msgbox Route120_Text_1BF43E, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_Jennifer:: @ 8151A74
	trainerbattle_single TRAINER_JENNIFER, Route120_Text_JenniferIntro, Route120_Text_JenniferDefeat
	msgbox Route120_Text_JenniferPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_Chip:: @ 8151A8B
	trainerbattle_single TRAINER_CHIP, Route120_Text_ChipIntro, Route120_Text_ChipDefeat
	msgbox Route120_Text_ChipPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_Clarissa:: @ 8151AA2
	trainerbattle_single TRAINER_CLARISSA, Route120_Text_ClarissaIntro, Route120_Text_ClarissaDefeat
	msgbox Route120_Text_ClarissaPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_Angelica:: @ 8151AB9
	trainerbattle_single TRAINER_ANGELICA, Route120_Text_AngelicaIntro, Route120_Text_AngelicaDefeat
	msgbox Route120_Text_AngelicaPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_Keigo:: @ 8151AD0
	trainerbattle_single TRAINER_KEIGO, Route120_Text_KeigoIntro, Route120_Text_KeigoDefeat
	msgbox Route120_Text_KeigoPostBattle, MSGBOX_AUTOCLOSE
	end

Route120_EventScript_151AE7:: @ 8151AE7
	trainerbattle_single TRAINER_TSUNAO, Route120_Text_1BF8C5, Route120_Text_1BF920
	msgbox Route120_Text_1BF946, MSGBOX_AUTOCLOSE
	end
