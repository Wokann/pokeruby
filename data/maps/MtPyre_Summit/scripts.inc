MtPyre_Summit_MapScripts:: @ 815D4B1
	map_script MAP_SCRIPT_ON_TRANSITION, MtPyre_Summit_OnTransition
	.byte 0

MtPyre_Summit_OnTransition:: @ 815D4B7
	call Common_EventScript_SetupEvilTeamGfxIds
	end

MtPyre_Summit_EventScript_EvilTeamStealsOrbTrigger0:: @ 815D4BD
	lockall
	setvar VAR_0x8008, 0
	goto MtPyre_Summit_EventScript_EvilTeamStealsOrb
	end

MtPyre_Summit_EventScript_EvilTeamStealsOrbTrigger1:: @ 815D4C9
	lockall
	setvar VAR_0x8008, 1
	goto MtPyre_Summit_EventScript_EvilTeamStealsOrb
	end

MtPyre_Summit_EventScript_EvilTeamStealsOrbTrigger2:: @ 815D4D5
	lockall
	setvar VAR_0x8008, 2
	goto MtPyre_Summit_EventScript_EvilTeamStealsOrb
	end

MtPyre_Summit_EventScript_EvilTeamStealsOrb:: @ 815D4E1
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	applymovement 2, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 50
	compare VAR_0x8008, 0
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayer0
	compare VAR_0x8008, 1
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayer1
	compare VAR_0x8008, 2
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayer2
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_ArchieClaimsRedOrb, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_MaxieClaimsBlueOrb, MSGBOX_DEFAULT
	.endif
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject 2
	removeobject 4
	removeobject 5
	removeobject 6
	setflag FLAG_HIDE_LEADER_MT_PYRE_SUMMIT
	setflag FLAG_HIDE_GRUNTS_MT_PYRE_SUMMIT
	fadescreen FADE_FROM_BLACK
	delay 20
	call MtPyre_Summit_EventScript_SetOrbStolenState
	compare VAR_0x8008, 0
	call_if_eq MtPyre_Summit_EventScript_OldLadyApproachPlayer0
	compare VAR_0x8008, 1
	call_if_eq MtPyre_Summit_EventScript_OldLadyApproachPlayer1
	compare VAR_0x8008, 2
	call_if_eq MtPyre_Summit_EventScript_OldLadyApproachPlayer2
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_OldLadyGivesBlueOrb, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_OldLadyGivesRedOrb, MSGBOX_DEFAULT
	.endif
	giveitem ITEM_RED_OR_BLUE_ORB
	setflag FLAG_RECEIVED_RED_OR_BLUE_ORB
	releaseall
	end

MtPyre_Summit_EventScript_SetOrbStolenState:: @ 815D57A
	setflag FLAG_MT_PYRE_ORB_STOLEN
	setvar VAR_SLATEPORT_STATE, 1
	clearflag FLAG_HIDE_STERN_SLATEPORT
	clearflag FLAG_HIDE_GABBY_AND_TY_SLATEPORT
	setvar VAR_SLATEPORT_HARBOR_STATE, 1
	setvar VAR_MT_PYRE_STATE, 1
	return

MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayer0:: @ 815D593
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderFacePlayer0
	waitmovement 0
	return

MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayer1:: @ 815D59E
	return

MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayer2:: @ 815D59F
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderFacePlayer2
	waitmovement 0
	return

MtPyre_Summit_EventScript_OldLadyApproachPlayer0:: @ 815D5AA
	applymovement 3, MtPyre_Summit_Movement_OldLadyApproachPlayer0
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	return

MtPyre_Summit_EventScript_OldLadyApproachPlayer1:: @ 815D5BF
	applymovement 3, MtPyre_Summit_Movement_OldLadyApproachPlayer1
	waitmovement 0
	return

MtPyre_Summit_EventScript_OldLadyApproachPlayer2:: @ 815D5CA
	applymovement 3, MtPyre_Summit_Movement_OldLadyApproachPlayer2
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	return

MtPyre_Summit_Movement_OldLadyApproachPlayer0:: @ 815D5DF
	walk_down
	walk_down
	walk_in_place_fastest_left
	step_end

MtPyre_Summit_Movement_OldLadyApproachPlayer1:: @ 815D5E3
	walk_down
	step_end

MtPyre_Summit_Movement_OldLadyApproachPlayer2:: @ 815D5E5
	walk_down
	walk_down
	walk_in_place_fastest_right
	step_end

MtPyre_Summit_Movement_EvilTeamLeaderFacePlayer0:: @ 815D5E9
	walk_left
	walk_in_place_fastest_down
	step_end

MtPyre_Summit_Movement_EvilTeamLeaderFacePlayer2:: @ 815D5EC
	walk_right
	walk_in_place_fastest_down
	step_end

MtPyre_Summit_EventScript_OldMan:: @ 815D5EF
	lock
	faceplayer
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_AquaOldManWillYouHearOutMyTale, MSGBOX_YESNO
	.else
	msgbox MtPyre_Summit_Text_MagmaOldManWillYouHearOutMyTale, MSGBOX_YESNO
	.endif
	compare VAR_RESULT, YES
	call_if_eq MtPyre_Summit_EventScript_OldManTale
	compare VAR_RESULT, NO
	call_if_eq MtPyre_Summit_EventScript_DeclineOldManTale
	release
	end

MtPyre_Summit_EventScript_OldManTale:: @ 815D611
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_AquaOldManSeaAndLandTale, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_MagmaOldManLandAndSeaTale, MSGBOX_DEFAULT
	.endif
	return

MtPyre_Summit_EventScript_DeclineOldManTale:: @ 815D61A
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_AquaOldManDeclineTale, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_MagmaOldManDeclineTale, MSGBOX_DEFAULT
	.endif
	return

MtPyre_Summit_EventScript_OldLady:: @ 815D623
	lock
	faceplayer
	goto_if_set FLAG_RETURNED_RED_OR_BLUE_ORB, MtPyre_Summit_EventScript_OldLadyOrbReturned
	goto_if_set FLAG_LEGENDARY_BATTLE_COMPLETED, MtPyre_Summit_EventScript_OldLadyAskReturnOrb
	goto_if_set FLAG_LEGEND_ESCAPED_SEAFLOOR_CAVERN, MtPyre_Summit_EventScript_OldLadyLegendaryAwakened
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_OldLadyWarnsRedOrbStolen, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_OldLadyWarnsBlueOrbStolen, MSGBOX_DEFAULT
	.endif
	release
	end

MtPyre_Summit_EventScript_OldLadyAskReturnOrb:: @ 815D64A
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_OldLadyAskReturnBlueOrb, MSGBOX_YESNO
	.else
	msgbox MtPyre_Summit_Text_OldLadyAskReturnRedOrb, MSGBOX_YESNO
	.endif
	compare VAR_RESULT, YES
	goto_if_eq MtPyre_Summit_EventScript_OldLadyReturnOrb
	compare VAR_RESULT, NO
	goto_if_eq MtPyre_Summit_EventScript_OldLadyAskAgainReturnOrb
	end

MtPyre_Summit_EventScript_OldLadyReturnOrb:: @ 815D669
	setvar VAR_0x8004, ITEM_RED_OR_BLUE_ORB
	call Common_EventScript_PlayerHandedOverTheItem
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	delay 40
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	delay 10
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_OldLadyThanksForReturningBlueOrb, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_OldLadyThanksForReturningRedOrb, MSGBOX_DEFAULT
	.endif
	setflag FLAG_RETURNED_RED_OR_BLUE_ORB
	release
	end

MtPyre_Summit_EventScript_OldLadyAskAgainReturnOrb:: @ 815D69B
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_OldLadyAskAgainReturnBlueOrb, MSGBOX_YESNO
	.else
	msgbox MtPyre_Summit_Text_OldLadyAskAgainReturnRedOrb, MSGBOX_YESNO
	.endif
	compare VAR_RESULT, YES
	goto_if_eq MtPyre_Summit_EventScript_OldLadyReturnOrb
	compare VAR_RESULT, NO
	goto_if_eq MtPyre_Summit_EventScript_OldLadyAskAgainReturnOrb
	end

MtPyre_Summit_EventScript_OldLadyLegendaryAwakened:: @ 815D6BA
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_OldLadyKyogreAwakened, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_OldLadyGroudonAwakened, MSGBOX_DEFAULT
	.endif
	release
	end

MtPyre_Summit_EventScript_OldLadyOrbReturned:: @ 815D6C4
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_OldLadyExplainsKyogre, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_OldLadyExplainsGroudon, MSGBOX_DEFAULT
	.endif
	release
	end

MtPyre_Summit_EventScript_EvilTeamReturnsOrbTrigger0:: @ 815D6CE
	lockall
	setvar VAR_0x8008, 0
	goto MtPyre_Summit_EventScript_EvilTeamReturnsOrb
	end

MtPyre_Summit_EventScript_EvilTeamReturnsOrbTrigger1:: @ 815D6DA
	lockall
	setvar VAR_0x8008, 1
	goto MtPyre_Summit_EventScript_EvilTeamReturnsOrb
	end

MtPyre_Summit_EventScript_EvilTeamReturnsOrbTrigger2:: @ 815D6E6
	lockall
	setvar VAR_0x8008, 2
	goto MtPyre_Summit_EventScript_EvilTeamReturnsOrb
	end

MtPyre_Summit_EventScript_EvilTeamReturnsOrb:: @ 815D6F2
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	applymovement 2, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 50
	compare VAR_0x8008, 0
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayerForReturn0
	compare VAR_0x8008, 1
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayerForReturn1
	compare VAR_0x8008, 2
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayerForReturn2
	.ifdef SAPPHIRE
	msgbox MtPyre_Summit_Text_ArchieReturnsRedOrb, MSGBOX_DEFAULT
	.else
	msgbox MtPyre_Summit_Text_MaxieReturnsBlueOrb, MSGBOX_DEFAULT
	.endif
	closemessage
	compare VAR_0x8008, 0
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderExit0
	compare VAR_0x8008, 1
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderExit1
	compare VAR_0x8008, 2
	call_if_eq MtPyre_Summit_EventScript_EvilTeamLeaderExit2
	removeobject 2
	setvar VAR_MT_PYRE_STATE, 3
	releaseall
	end

MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayerForReturn0:: @ 815D75E
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderFacePlayerForReturn0
	waitmovement 0
	return

MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayerForReturn1:: @ 815D769
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderFacePlayerForReturn1
	waitmovement 0
	return

MtPyre_Summit_EventScript_EvilTeamLeaderFacePlayerForReturn2:: @ 815D774
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderFacePlayerForReturn2
	waitmovement 0
	return

MtPyre_Summit_EventScript_EvilTeamLeaderExit0:: @ 815D77F
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderExit0
	waitmovement 0
	return

MtPyre_Summit_EventScript_EvilTeamLeaderExit1:: @ 815D78A
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderExit1
	waitmovement 0
	return

MtPyre_Summit_EventScript_EvilTeamLeaderExit2:: @ 815D795
	applymovement 2, MtPyre_Summit_Movement_EvilTeamLeaderExit2
	waitmovement 0
	return

MtPyre_Summit_Movement_EvilTeamLeaderFacePlayerForReturn0:: @ 815D7A0
	walk_left
	walk_in_place_fastest_down
	step_end

MtPyre_Summit_Movement_EvilTeamLeaderFacePlayerForReturn1:: @ 815D7A3
	step_end

MtPyre_Summit_Movement_EvilTeamLeaderFacePlayerForReturn2:: @ 815D7A4
	walk_right
	walk_in_place_fastest_down
	step_end

MtPyre_Summit_Movement_EvilTeamLeaderExit0:: @ 815D7A7
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtPyre_Summit_Movement_EvilTeamLeaderExit1:: @ 815D7B0
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtPyre_Summit_Movement_EvilTeamLeaderExit2:: @ 815D7B9
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end
