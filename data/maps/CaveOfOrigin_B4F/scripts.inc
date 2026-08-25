CaveOfOrigin_B4F_MapScripts:: @ 815DDAC
	map_script MAP_SCRIPT_ON_RESUME, CaveOfOrigin_B4F_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, CaveOfOrigin_B4F_OnTransition
	.byte 0

CaveOfOrigin_B4F_OnResume:: @ 815DDB7
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, CaveOfOrigin_B4F_EventScript_RemoveAwakenedLegendary
	end

CaveOfOrigin_B4F_EventScript_RemoveAwakenedLegendary:: @ 815DDC1
	removeobject 1
	return

CaveOfOrigin_B4F_OnTransition:: @ 815DDC5
	.ifdef SAPPHIRE
	setmaplayoutindex 163
	.else
	setmaplayoutindex 313
	.endif
	call Common_EventScript_SetupLegendaryGfxIds
	call_if_set FLAG_LEGENDARY_BATTLE_COMPLETED, CaveOfOrigin_EventScript_DisableTriggers
	end

CaveOfOrigin_B4F_EventScript_AwakenLegendary:: @ 815DDD7
	lockall
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	.ifdef SAPPHIRE
	msgbox CaveOfOrigin_B4F_Text_BlueOrbGlows, MSGBOX_DEFAULT
	.else
	msgbox CaveOfOrigin_B4F_Text_RedOrbGlows, MSGBOX_DEFAULT
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
	special sub_80818A4
	waitstate
	special sub_80818FC
	waitstate
	applymovement 1, CaveOfOrigin_B4F_Movement_AwakenedLegendaryDescends
	waitmovement 0
	delay 60
	setwildbattle SPECIES_GROUDON_OR_KYOGRE, 45, ITEM_NONE
	waitse
	playmoncry SPECIES_GROUDON_OR_KYOGRE, 2
	delay 40
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartGroudonKyogreBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, 1
	call_if_eq CaveOfOrigin_B4F_EventScript_LegendaryReturnsToSleep
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

CaveOfOrigin_B4F_EventScript_LegendaryReturnsToSleep:: @ 815DE73
	.ifdef SAPPHIRE
	msgbox CaveOfOrigin_B4F_Text_KyogreReturnsToSleep, MSGBOX_DEFAULT
	.else
	msgbox CaveOfOrigin_B4F_Text_GroudonReturnsToSleep, MSGBOX_DEFAULT
	.endif
	closemessage
	return

CaveOfOrigin_B4F_Movement_AwakenedLegendaryDescends:: @ 815DE7D
	init_affine_anim
	walk_down_start_affine
	delay_16
	delay_16
	walk_down_affine
	step_end
