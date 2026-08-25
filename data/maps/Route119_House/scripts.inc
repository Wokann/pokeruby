Route119_House_MapScripts:: @ 8163DDD
	.byte 0

Route119_House_EventScript_Woman:: @ 8163DDE
	msgbox Route119_House_Text_RumorAboutCaveOfOrigin, MSGBOX_NPC
	end

Route119_House_EventScript_Wingull:: @ 8163DE7
	lock
	faceplayer
	waitse
	playmoncry SPECIES_WINGULL, 0
	msgbox Route119_House_Text_Wingull, MSGBOX_DEFAULT
	waitmoncry
	release
	end
