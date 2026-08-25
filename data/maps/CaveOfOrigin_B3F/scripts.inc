CaveOfOrigin_B3F_MapScripts:: @ 815DD9C
	map_script MAP_SCRIPT_ON_TRANSITION, CaveOfOrigin_B3F_OnTransition
	.byte 0

CaveOfOrigin_B3F_OnTransition:: @ 815DDA2
	call_if_set FLAG_LEGENDARY_BATTLE_COMPLETED, CaveOfOrigin_EventScript_DisableTriggers
	end
