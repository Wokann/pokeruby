Route101_MapScripts:: @ 814E924
	map_script MAP_SCRIPT_ON_TRANSITION, Route101_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route101_OnFrame
	.byte 0

Route101_OnTransition:: @ 814E92F
	call ProfBirch_EventScript_UpdateLocation
	end

Route101_OnFrame:: @ 814E935
	map_script_2 VAR_ROUTE101_STATE, 0, Route101_EventScript_HideMapNamePopup
	.2byte 0

Route101_EventScript_HideMapNamePopup:: @ 814E93F
	setflag FLAG_HIDE_MAP_NAME_POPUP
	setvar VAR_ROUTE101_STATE, 1
	end

Route101_EventScript_StartBirchRescue:: @ 814E948
	lockall
	playbgm MUS_HELP, TRUE
	msgbox Route101_Text_HelpMe, MSGBOX_DEFAULT
	closemessage
	setobjectxy LOCALID_ROUTE101_BIRCH, 0, 15
	setobjectxy LOCALID_ROUTE101_POOCHYENA, 0, 16
	applymovement LOCALID_PLAYER, Route101_Movement_EnterScene
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchRunAway1
	applymovement LOCALID_ROUTE101_POOCHYENA, Route101_Movement_PoochyenaChase1
	waitmovement 0
	applymovement LOCALID_ROUTE101_POOCHYENA, Route101_Movement_PoochyenaChaseInCircles
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchRunInCircles
	waitmovement 0
	applymovement LOCALID_ROUTE101_BIRCH, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	applymovement LOCALID_ROUTE101_POOCHYENA, Route101_Movement_PoochyenaFaceBirch
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchFacePoochyena
	waitmovement 0
	msgbox Route101_Text_PleaseHelp, MSGBOX_DEFAULT
	closemessage
	setvar VAR_ROUTE101_STATE, 2
	releaseall
	end

Route101_EventScript_PreventExitSouth:: @ 814E9B8
	lockall
	msgbox Route101_Text_DontLeaveMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route101_Movement_PreventExitSouth
	waitmovement 0
	releaseall
	end

Route101_EventScript_PreventExitWest:: @ 814E9CE
	lockall
	msgbox Route101_Text_DontLeaveMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route101_Movement_PreventExitWest
	waitmovement 0
	releaseall
	end

Route101_EventScript_PreventExitNorth:: @ 814E9E4
	lockall
	msgbox Route101_Text_DontLeaveMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route101_Movement_PreventExitNorth
	waitmovement 0
	releaseall
	end

Route101_Movement_PreventExitSouth:: @ 814E9FA
	walk_up
	step_end

Route101_Movement_PreventExitWest:: @ 814E9FC
	walk_right
	step_end

Route101_Movement_PreventExitNorth:: @ 814E9FE
	walk_down
	step_end

Route101_Movement_PoochyenaChaseInCircles:: @ 814EA00
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	step_end

Route101_Movement_PoochyenaChase1:: @ 814EA1F
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	step_end

@ 814EA26
	step_end

Route101_Movement_PoochyenaFaceBirch:: @ 814EA27
	walk_in_place_fast_left
	walk_in_place_fast_left
	walk_in_place_fast_left
	walk_in_place_fast_left
	step_end

Route101_Movement_EnterScene:: @ 814EA2C
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route101_Movement_BirchRunInCircles:: @ 814EA31
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

Route101_Movement_BirchRunAway1:: @ 814EA50
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	step_end

@ 814EA57
	step_end

Route101_Movement_BirchFacePoochyena:: @ 814EA58
	walk_in_place_fast_right
	walk_in_place_fast_right
	walk_in_place_fast_right
	walk_in_place_fast_right
	step_end

Route101_Movement_Unused1:: @ 814EA5D
	walk_up
	walk_up
	step_end

Route101_Movement_Unused2:: @ 814EA60
	walk_up
	walk_left
	walk_up
	step_end

Route101_EventScript_Youngster:: @ 814EA64
	msgbox Route101_Text_TakeTiredPokemonToPokeCenter, MSGBOX_NPC
	end

Route101_EventScript_Boy:: @ 814EA6D
	msgbox Route101_Text_WildPokemonInTallGrass, MSGBOX_NPC
	end

Route101_EventScript_RouteSign:: @ 814EA76
	msgbox Route101_Text_RouteSign, MSGBOX_SIGN
	end

Route101_EventScript_BirchsBag:: @ 814EA7F
	lock
	faceplayer
	setflag FLAG_SYS_POKEMON_GET
	setflag FLAG_RESCUED_BIRCH
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_ROUTE101_POOCHYENA
	setobjectxy LOCALID_PLAYER, 6, 13
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	special ScrSpecial_ChooseStarter
	waitstate
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchApproachPlayer
	waitmovement 0
	msgbox Route101_Text_YouSavedMe, MSGBOX_DEFAULT
	special ScrSpecial_HealPlayerParty
	setflag FLAG_HIDE_BIRCH_BATTLE_POOCHYENA
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH
	setflag FLAG_HIDE_BIRCH_STARTERS_BAG
	setvar VAR_BIRCH_LAB_STATE, 2
	setvar VAR_ROUTE101_STATE, 3
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 255, 6, 5
	waitstate
	release
	end

Route101_Movement_BirchApproachPlayer:: @ 814EAD7
	walk_right
	step_end
