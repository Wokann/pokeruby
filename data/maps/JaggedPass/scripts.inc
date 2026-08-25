JaggedPass_MapScripts:: @ 815D266
	map_script MAP_SCRIPT_ON_RESUME, JaggedPass_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, JaggedPass_OnTransition
	.byte 0

JaggedPass_OnResume:: @ 815D271
	setstepcallback 1
	end

JaggedPass_OnTransition:: @ 815D274
	compare VAR_JAGGED_PASS_ASH_WEATHER, 1
	call_if_eq JaggedPass_EventScript_SetWeatherAsh
	end

JaggedPass_EventScript_SetWeatherAsh:: @ 815D280
	setweather WEATHER_ASH
	doweather
	return

JaggedPass_EventScript_Eric:: @ 815D285
	trainerbattle_single TRAINER_ERIC, JaggedPass_Text_EricIntro, JaggedPass_Text_EricDefeat
	msgbox JaggedPass_Text_EricPostBattle, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_Diana:: @ 815D29C
	trainerbattle_single TRAINER_DIANA_1, JaggedPass_Text_DianaIntro, JaggedPass_Text_DianaDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq JaggedPass_EventScript_DianaRematch
	msgbox JaggedPass_Text_DianaPostBattle, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_DianaRematch:: @ 815D2C3
	trainerbattle_rematch TRAINER_DIANA_1, JaggedPass_Text_DianaRematchIntro, JaggedPass_Text_DianaRematchDefeat
	msgbox JaggedPass_Text_DianaPostRematch, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_Ethan:: @ 815D2DA
	trainerbattle_single TRAINER_ETHAN_1, JaggedPass_Text_EthanIntro, JaggedPass_Text_EthanDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq JaggedPass_EventScript_EthanRematch
	msgbox JaggedPass_Text_EthanPostBattle, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_EthanRematch:: @ 815D301
	trainerbattle_rematch TRAINER_ETHAN_1, JaggedPass_Text_EthanRematchIntro, JaggedPass_Text_EthanRematchDefeat
	msgbox JaggedPass_Text_EthanPostRematch, MSGBOX_AUTOCLOSE
	end
