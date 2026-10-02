LittlerootTown_BrendansHouse_2F_MapScripts:: @ 81527A4
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_BrendansHouse_2F_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, LittlerootTown_BrendansHouse_2F_OnWarp
	.byte 0

LittlerootTown_BrendansHouse_2F_OnTransition:: @ 81527AF
	call_if_unset FLAG_DEFEATED_RIVAL_ROUTE103, LittlerootTown_BrendansHouse_2F_EventScript_RivalCheckingNotebook
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 4, PlayersHouse_2F_EventScript_BlockStairsUntilClockIsSet
	call SecretBase_EventScript_SetDecorationFlags
	setvar VAR_SECRET_BASE_INITIALIZED, 0
	end

LittlerootTown_BrendansHouse_2F_EventScript_RivalCheckingNotebook:: @ 81527CE
	setobjectxyperm LOCALID_RIVALS_HOUSE_2F_RIVAL, 1, 2
	setobjectmovementtype LOCALID_RIVALS_HOUSE_2F_RIVAL, MOVEMENT_TYPE_FACE_UP
	return

LittlerootTown_BrendansHouse_2F_OnWarp:: @ 81527DA
	map_script_2 VAR_SECRET_BASE_INITIALIZED, 0, LittlerootTown_BrendansHouse_2F_EventScript_CheckInitDecor
	.2byte 0

LittlerootTown_BrendansHouse_2F_EventScript_CheckInitDecor:: @ 81527E4
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, SecretBase_EventScript_InitDecorations
	end

PlayersHouse_2F_EventScript_Bookshelf:: @ 81527F1
	lockall
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, PlayersHouse_2F_EventScript_PlayerBookshelf
	goto_if_eq VAR_RESULT, FEMALE, RivalsHouse_2F_EventScript_PictureBookshelf
	end

PlayersHouse_2F_EventScript_PlayerBookshelf:: @ 815280A
	msgbox PlayersHouse_2F_Text_Bookshelf, MSGBOX_DEFAULT
	releaseall
	end

RivalsHouse_2F_EventScript_PictureBookshelf:: @ 8152814
	msgbox RivalsHouse_2F_Text_PictureBookshelf, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_PC:: @ 815281E
	lockall
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LittlerootTown_BrendansHouse_2F_EventScript_CheckPlayersPC
	goto_if_eq VAR_RESULT, FEMALE, LittlerootTown_BrendansHouse_2F_EventScript_CheckRivalsPC
	end

LittlerootTown_BrendansHouse_2F_EventScript_CheckPlayersPC:: @ 8152837
	setvar VAR_0x8004, PC_LOCATION_BRENDANS_HOUSE
	special DoPCTurnOnEffect
	playse SE_PC_ON
	msgbox gText_PlayerHouseBootPC, MSGBOX_DEFAULT
	special BedroomPC
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_TurnOffPlayerPC:: @ 8152850
	setvar VAR_0x8004, PC_LOCATION_BRENDANS_HOUSE
	playse SE_PC_OFF
	special DoPCTurnOffEffect
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_CheckRivalsPC:: @ 815285D
	msgbox gText_PokemonTrainerSchoolEmail, MSGBOX_DEFAULT
	releaseall
	end

PlayersHouse_2F_EventScript_Notebook:: @ 8152867
	msgbox PlayersHouse_2F_Text_Notebook, MSGBOX_SIGN
	end

PlayersHouse_2F_EventScript_GameCube:: @ 8152870
	msgbox PlayersHouse_2F_Text_ItsAGameCube, MSGBOX_SIGN
	end
