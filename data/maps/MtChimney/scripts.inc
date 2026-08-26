MtChimney_MapScripts:: @ 815CF7C
	map_script MAP_SCRIPT_ON_RESUME, MtChimney_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, MtChimney_OnTransition
	.byte 0

MtChimney_OnTransition:: @ 815CF87
	setvar VAR_JAGGED_PASS_ASH_WEATHER, 1
	call Common_EventScript_SetupEvilTeamGfxIds
	end

MtChimney_OnResume:: @ 815CF92
	setstepcallback STEP_CB_ASH
	end

MtChimney_EventScript_LavaCookieLady:: @ 815CF95
	lock
	faceplayer
	showmoneybox 0, 0
	nop
	msgbox MtChimney_Text_LavaCookiesJust200, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MtChimney_EventScript_DeclineLavaCookie
	checkmoney 0xc8, 0
	compare VAR_RESULT, 0
	goto_if_eq MtChimney_EventScript_NotEnoughMoney
	msgbox MtChimney_Text_ThankYouDear, MSGBOX_DEFAULT
	checkitemspace ITEM_LAVA_COOKIE, 1
	compare VAR_RESULT, 1
	call_if_eq MtChimney_EventScript_RemoveMoney
	giveitem ITEM_LAVA_COOKIE
	compare VAR_RESULT, 0
	goto_if_eq MtChimney_EventScript_BagIsFull
	hidemoneybox 0, 0
	release
	end

MtChimney_EventScript_BagIsFull:: @ 815CFF3
	msgbox Text_TooBadBagIsFull, MSGBOX_DEFAULT
	hidemoneybox 0, 0
	release
	end

MtChimney_EventScript_RemoveMoney:: @ 815D000
	removemoney 200, 0
	updatemoneybox 0, 0
	nop
	return

MtChimney_EventScript_DeclineLavaCookie:: @ 815D00B
	msgbox MtChimney_Text_OhFineThen, MSGBOX_DEFAULT
	hidemoneybox 0, 0
	release
	end

MtChimney_EventScript_NotEnoughMoney:: @ 815D018
	msgbox MtChimney_Text_YouveNotGotTheMoney, MSGBOX_DEFAULT
	hidemoneybox 0, 0
	release
	end

MtChimney_Movement_OtherTeamLeaderApproachPlayerEast:: @ 815D025
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_right
	step_end

MtChimney_Movement_OtherTeamLeaderExitEast:: @ 815D02D
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtChimney_Movement_OtherTeamLeaderApproachPlayerNorth:: @ 815D036
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_right
	walk_right
	step_end

MtChimney_Movement_OtherTeamLeaderExitNorth:: @ 815D03E
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtChimney_Movement_Unused1:: @ 815D048
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	step_end

MtChimney_Movement_Unused2:: @ 815D05A
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_down
	step_end

MtChimney_Movement_Unused3:: @ 815D063
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtChimney_Movement_Unused4:: @ 815D072
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	step_end

MtChimney_Movement_Unused5:: @ 815D08B
	delay_16
	delay_16
	delay_16
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_down
	step_end

MtChimney_Movement_Unused6:: @ 815D096
	delay_16
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtChimney_Movement_Unused7:: @ 815D0A6
	delay_16
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	step_end

MtChimney_Movement_Unused8:: @ 815D0B7
	delay_16
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_in_place_fastest_down
	step_end

MtChimney_Movement_Unused9:: @ 815D0BF
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtChimney_Movement_Unused10:: @ 815D0CD
	walk_down
	step_end

MtChimney_Movement_Unused11:: @ 815D0CF
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtChimney_Movement_Unused12:: @ 815D0D7
	delay_16
	delay_16
	walk_in_place_fastest_left
	delay_16
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtChimney_Movement_Unused13:: @ 815D0E1
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	walk_in_place_fastest_left
	delay_16
	walk_down
	walk_down
	step_end

MtChimney_Movement_Unused14:: @ 815D0EC
	lock_facing_direction
	walk_fast_left
	unlock_facing_direction
	face_right
	step_end

MtChimney_Movement_Unused15:: @ 815D0F1
	walk_left
	walk_in_place_fastest_right
	delay_16
	step_end

MtChimney_EventScript_BusyEvilTeamGrunt1:: @ 815D0F5
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_MagmaGruntAquaOutnumbersUs, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_AquaGruntMagmaOutnumbersUs, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_BusyEvilTeamGrunt2:: @ 815D0FE
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_MagmaGruntLessHabitatForPokemon, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_AquaGruntLessHabitatForWaterPokemon, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_BusyEvilTeamGrunt3:: @ 815D107
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_MagmaGruntAquaPlanInsane, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_AquaGruntMagmaPlanInsane, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_EvilTeamAdmin:: @ 815D110
	.ifdef SAPPHIRE
	trainerbattle_single TRAINER_MT_CHIMNEY_GRUNT_1, MtChimney_Text_MattIntro, MtChimney_Text_MattDefeat
	msgbox MtChimney_Text_MattPostBattle, MSGBOX_AUTOCLOSE
	.else
	trainerbattle_single TRAINER_MT_CHIMNEY_GRUNT_1, MtChimney_Text_TabithaIntro, MtChimney_Text_TabithaDefeat
	msgbox MtChimney_Text_TabithaPostBattle, MSGBOX_AUTOCLOSE
	.endif
	end

MtChimney_EventScript_EvilTeamGrunt:: @ 815D127
	.ifdef SAPPHIRE
	trainerbattle_single TRAINER_MT_CHIMNEY_GRUNT_2, MtChimney_Text_AquaGrunt2Intro, MtChimney_Text_AquaGrunt2Defeat
	msgbox MtChimney_Text_AquaGrunt2PostBattle, MSGBOX_AUTOCLOSE
	.else
	trainerbattle_single TRAINER_MT_CHIMNEY_GRUNT_2, MtChimney_Text_MagmaGrunt2Intro, MtChimney_Text_MagmaGrunt2Defeat
	msgbox MtChimney_Text_MagmaGrunt2PostBattle, MSGBOX_AUTOCLOSE
	.endif
	end

MtChimney_EventScript_BusyEvilTeamGrunt4:: @ 815D13E
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_AquaGruntTeamMagmaInterfering, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_MagmaGruntTeamAquaInterfering, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_BusyEvilTeamGrunt5:: @ 815D147
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_AquaGruntMeteoritesPower, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_MagmaGruntMeteoritesPower, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_BusyEvilTeamGrunt6:: @ 815D150
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_AquaGruntExpandSea, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_MagmaGruntExpandLand, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_BusyEvilTeamGrunt7:: @ 815D159
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_AquaGruntEnlargeSea, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_MagmaGruntEnlargeLand, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_BusyEvilTeamGrunt8:: @ 815D162
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_AquaGruntHoseThemDown, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_MagmaGruntDouseThemInFire, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_EvilTeamPoochyena1:: @ 815D16B
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_AquaPoochyena1Cry, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_MagmaPoochyena1Cry, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_EvilTeamPoochyena2:: @ 815D174
	.ifdef SAPPHIRE
	msgbox MtChimney_Text_MagmaPoochyena2Cry, MSGBOX_SIGN
	.else
	msgbox MtChimney_Text_AquaPoochyena2Cry, MSGBOX_SIGN
	.endif
	end

MtChimney_EventScript_MeteoriteMachine:: @ 815D17D
	lockall
	goto_if_unset FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, MtChimney_EventScript_MachineOn
	goto_if_set FLAG_RECEIVED_METEORITE, MtChimney_EventScript_MachineOff
	msgbox MtChimney_Text_RemoveTheMeteorite, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MtChimney_EventScript_LeaveMeteoriteAlone
	msgbox MtChimney_Text_PlayerRemovedMeteorite, MSGBOX_DEFAULT
	giveitem ITEM_METEORITE
	setflag FLAG_RECEIVED_METEORITE
	releaseall
	end

MtChimney_EventScript_LeaveMeteoriteAlone:: @ 815D1BC
	msgbox MtChimney_Text_PlayerLeftMeteorite, MSGBOX_DEFAULT
	releaseall
	end

MtChimney_EventScript_MachineOff:: @ 815D1C6
	msgbox MtChimney_Text_MachineMakesNoResponse, MSGBOX_DEFAULT
	releaseall
	end

MtChimney_EventScript_MachineOn:: @ 815D1D0
	msgbox MtChimney_Text_MeteoriteFittedOnMachine, MSGBOX_DEFAULT
	releaseall
	end

MtChimney_EventScript_RouteSign:: @ 815D1DA
	msgbox MtChimney_Text_RouteSign, MSGBOX_SIGN
	end

MtChimney_EventScript_Shelby:: @ 815D1E3
	trainerbattle_single TRAINER_SHELBY_1, MtChimney_Text_ShelbyIntro, MtChimney_Text_ShelbyDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq MtChimney_EventScript_RematchShelby
	msgbox MtChimney_Text_ShelbyPostBattle, MSGBOX_AUTOCLOSE
	end

MtChimney_EventScript_RematchShelby:: @ 815D20A
	trainerbattle_rematch TRAINER_SHELBY_1, MtChimney_Text_ShelbyRematchIntro, MtChimney_Text_ShelbyRematchDefeat
	msgbox MtChimney_Text_ShelbyPostRematch, MSGBOX_AUTOCLOSE
	end

MtChimney_EventScript_Melissa:: @ 815D221
	trainerbattle_single TRAINER_MELISSA, MtChimney_Text_MelissaIntro, MtChimney_Text_MelissaDefeat
	msgbox MtChimney_Text_MelissaPostBattle, MSGBOX_AUTOCLOSE
	end

MtChimney_EventScript_Sheila:: @ 815D238
	trainerbattle_single TRAINER_SHEILA, MtChimney_Text_SheilaIntro, MtChimney_Text_SheilaDefeat
	msgbox MtChimney_Text_SheilaPostBattle, MSGBOX_AUTOCLOSE
	end

MtChimney_EventScript_Shirley:: @ 815D24F
	trainerbattle_single TRAINER_SHIRLEY, MtChimney_Text_ShirleyIntro, MtChimney_Text_ShirleyDefeat
	msgbox MtChimney_Text_ShirleyPostBattle, MSGBOX_AUTOCLOSE
	end
