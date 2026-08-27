CaveOfOrigin_B4F_MapScripts:: @ 815DDAC
	map_script MAP_SCRIPT_ON_RESUME, CaveOfOrigin_B4F_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, CaveOfOrigin_B4F_OnTransition
	.byte 0

CaveOfOrigin_B4F_OnResume:: @ 815DDB7
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, CaveOfOrigin_B4F_EventScript_RemoveLegendary
	end

CaveOfOrigin_B4F_EventScript_RemoveLegendary:: @ 815DDC1
	removeobject LOCALID_CAVE_OF_ORIGIN_LEGENDARY
	return

CaveOfOrigin_B4F_OnTransition:: @ 815DDC5
	.ifdef SAPPHIRE
	setmaplayoutindex LAYOUT_CAVE_OF_ORIGIN_B4F
	.else
	setmaplayoutindex LAYOUT_CAVE_OF_ORIGIN_B4F_GROUDON
	.endif
	call Common_EventScript_SetupLegendaryGfxIds
	call_if_set FLAG_LEGENDARY_BATTLE_COMPLETED, CaveOfOrigin_EventScript_DisableTriggers
	end

CaveOfOrigin_B4F_EventScript_StartLegendaryBattle:: @ 815DDD7
	lockall
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	.ifdef SAPPHIRE
	msgbox CaveOfOrigin_B4F_Text_BlueOrbBeginsGlowing, MSGBOX_DEFAULT
	.else
	msgbox CaveOfOrigin_B4F_Text_RedOrbBeginsGlowing, MSGBOX_DEFAULT
	.endif
	closemessage
	setweather WEATHER_NONE
	doweather
	special WaitWeather
	waitstate
	playse SE_M_DETECT
	setfieldeffectargument 0, 9
	setfieldeffectargument 1, 13
	setfieldeffectargument 2, 0
	dofieldeffect FLDEFF_SPARKLE
	waitfieldeffect FLDEFF_SPARKLE
	.ifdef SAPPHIRE
	setvar VAR_RESULT, 3
	.else
	setvar VAR_RESULT, 2
	.endif
	playse SE_ORB
	special DoOrbEffect
	waitstate
	special FadeOutOrbEffect
	waitstate
	applymovement LOCALID_CAVE_OF_ORIGIN_LEGENDARY, CaveOfOrigin_B4F_Movement_LegendaryAppear
	waitmovement 0
	delay 60
	setwildbattle SPECIES_GROUDON_OR_KYOGRE, 45, ITEM_NONE
	waitse
	playmoncry SPECIES_GROUDON_OR_KYOGRE, 2
	delay 40
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special StartGroudonKyogreBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	call_if_eq CaveOfOrigin_B4F_EventScript_HandleDefeatedLegendary
	setflag FLAG_HIDE_WALLACE_SOOTOPOLIS
	setflag FLAG_HIDE_CAVE_OF_ORIGIN_ENTRANCE_WOMAN_1
	setflag FLAG_HIDE_CAVE_OF_ORIGIN_ENTRANCE_WOMAN_2
	clearflag FLAG_HIDE_WALLACE_SOOTOPOLIS_GYM
	setvar VAR_CAVE_OF_ORIGIN_B4F_STATE, 1
	setflag FLAG_LEGENDARY_BATTLE_COMPLETED
	clearflag FLAG_HIDE_SOOTOPOLIS_RESIDENTS
	clearflag FLAG_SYS_WEATHER_CTRL
	setvar VAR_MT_PYRE_STATE, 2
	clearflag FLAG_HIDE_LEADER_MT_PYRE_SUMMIT
	releaseall
	end

CaveOfOrigin_B4F_EventScript_HandleDefeatedLegendary:: @ 815DE73
	.ifdef SAPPHIRE
	msgbox CaveOfOrigin_B4F_Text_KyogreSettled, MSGBOX_DEFAULT
	.else
	msgbox CaveOfOrigin_B4F_Text_GroudonSettled, MSGBOX_DEFAULT
	.endif
	closemessage
	return

CaveOfOrigin_B4F_Movement_LegendaryAppear:: @ 815DE7D
	init_affine_anim
	walk_down_start_affine
	delay_16
	delay_16
	walk_down_affine
	step_end
