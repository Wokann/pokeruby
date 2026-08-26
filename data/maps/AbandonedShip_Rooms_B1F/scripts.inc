AbandonedShip_Rooms_B1F_MapScripts:: @ 815EA35
	map_script MAP_SCRIPT_ON_RESUME, AbandonedShip_Rooms_B1F_OnResume
	.byte 0

AbandonedShip_Rooms_B1F_OnResume:: @ 815EA3B
	setdivewarp MAP_ABANDONED_SHIP_UNDERWATER2, WARP_ID_NONE, 17, 4
	end

AbandonedShip_Rooms_B1F_EventScript_FatMan:: @ 815EA44
	msgbox AbandonedShip_Rooms_B1F_Text_GettingQueasy, MSGBOX_NPC
	end
