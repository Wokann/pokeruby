CaveOfOrigin_B1F_MapScripts:: @ 815DD7C
	map_script MAP_SCRIPT_ON_TRANSITION, CaveOfOrigin_B1F_OnTransition
	.byte 0

CaveOfOrigin_B1F_OnTransition:: @ 815DD82
	call_if_set FLAG_LEGENDARY_BATTLE_COMPLETED, CaveOfOrigin_EventScript_DisableTriggers
	end
