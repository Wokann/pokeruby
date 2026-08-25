MtPyre_Exterior_MapScripts:: @ 815D486
	map_script MAP_SCRIPT_ON_TRANSITION, MtPyre_Exterior_OnTransition
	.byte 0

MtPyre_Exterior_OnTransition:: @ 815D48C
	call MtPyre_Exterior_EventScript_CheckEnterFromSummit
	end

MtPyre_Exterior_EventScript_CheckEnterFromSummit:: @ 815D492
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	compare VAR_TEMP_1, 12
	goto_if_lt MtPyre_Exterior_EventScript_EnterFromSummit
	return

MtPyre_Exterior_EventScript_EnterFromSummit:: @ 815D4A3
	setweather WEATHER_FOG_1
	return

MtPyre_Exterior_EventScript_FogTrigger:: @ 815D4A7
	setweather WEATHER_FOG_1
	doweather
	end

MtPyre_Exterior_EventScript_SunTrigger:: @ 815D4AC
	setweather WEATHER_SUNNY
	doweather
	end
