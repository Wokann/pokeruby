Underwater_SealedChamber_MapScripts:: @ 815F080
	map_script MAP_SCRIPT_ON_DIVE_WARP, Underwater_SealedChamber_OnDive
	.byte 0

Underwater_SealedChamber_OnDive:: @ 815F086
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 12
	goto_if_ne Underwater_SealedChamber_EventScript_SurfaceRoute134
	compare VAR_0x8005, 44
	goto_if_ne Underwater_SealedChamber_EventScript_SurfaceRoute134
	goto Underwater_SealedChamber_EventScript_SurfaceSealedChamber

Underwater_SealedChamber_EventScript_SurfaceRoute134:: @ 815F0A6
	setdivewarp MAP_ROUTE134, 255, 60, 31
	end

Underwater_SealedChamber_EventScript_SurfaceSealedChamber:: @ 815F0AF
	setdivewarp MAP_SEALED_CHAMBER_OUTER_ROOM, 255, 10, 19
	end

Underwater_SealedChamber_EventScript_Braille:: @ 815F0B8
	lockall
	braillemessage Underwater_SealedChamber_Braille_GoUpHere
	waitbuttonpress
	erasebox 0, 0, 29, 19
	releaseall
	end
