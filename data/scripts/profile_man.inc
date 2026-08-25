ProfileMan_EventScript_Man:: @ 81B1B14
	lock
	faceplayer
	goto_if_set FLAG_SYS_CHAT_USED, ProfileMan_EventScript_GivenProfileBefore
	msgbox ProfileMan_Text_CollectTrainerProfiles, MSGBOX_DEFAULT
	goto ProfileMan_EventScript_AskToSeeProfile
	end

ProfileMan_EventScript_AskToSeeProfile:: @ 81B1B2D
	msgbox ProfileMan_Text_MayISeeYourProfile, MSGBOX_DEFAULT
	multichoice 17, 6, 20, 0
	switch VAR_RESULT
	case 0, ProfileMan_EventScript_CreateProfile
	case 1, ProfileMan_EventScript_DeclineShowProfile
	case 2, ProfileMan_EventScript_Info
	case 127, ProfileMan_EventScript_DeclineShowProfile
	end

ProfileMan_EventScript_Info:: @ 81B1B6C
	msgbox ProfileMan_Text_EasyChatExplanation, MSGBOX_DEFAULT
	goto ProfileMan_EventScript_AskToSeeProfile
	end

ProfileMan_EventScript_CreateProfile:: @ 81B1B7A
	msgbox ProfileMan_Text_LetsSeeItThen, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 0
	call Common_EventScript_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_0x8004, 1
	call_if_eq ProfileMan_EventScript_TryUnlockMysteryEventClub
	compare VAR_RESULT, 0
	goto_if_eq ProfileMan_EventScript_CancelShowProfile
	compare VAR_RESULT, 1
	goto_if_eq ProfileMan_EventScript_ShowProfile
	end

ProfileMan_EventScript_CancelShowProfile:: @ 81B1BB1
	msgbox ProfileMan_Text_NotIntoItRightNow, MSGBOX_DEFAULT
	release
	end

ProfileMan_EventScript_ShowProfile:: @ 81B1BBB
	setvar VAR_0x8004, 0
	special sub_80EB7C4
	waitmessage
	delay 80
	msgbox ProfileMan_Text_FantasticProfile, MSGBOX_DEFAULT
	release
	end

ProfileMan_EventScript_DeclineShowProfile:: @ 81B1BD1
	msgbox ProfileMan_Text_ImagineYouWouldHaveWonderfulProfile, MSGBOX_DEFAULT
	release
	end

ProfileMan_EventScript_GivenProfileBefore:: @ 81B1BDB
	msgbox ProfileMan_Text_YouHaveWonderfulSmile, MSGBOX_DEFAULT
	goto ProfileMan_EventScript_AskToSeeNewProfile
	end

ProfileMan_EventScript_AskToSeeNewProfile:: @ 81B1BE9
	msgbox ProfileMan_Text_MayISeeYourNewProfile, MSGBOX_DEFAULT
	multichoice 17, 6, 20, 0
	switch VAR_RESULT
	case 0, ProfileMan_EventScript_CreateNewProfile
	case 1, ProfileMan_EventScript_DeclineNewProfile
	case 2, ProfileMan_EventScript_InfoNewProfile
	case 127, ProfileMan_EventScript_DeclineNewProfile
	end

ProfileMan_EventScript_InfoNewProfile:: @ 81B1C28
	msgbox ProfileMan_Text_EasyChatExplanation, MSGBOX_DEFAULT
	goto ProfileMan_EventScript_AskToSeeNewProfile
	end

ProfileMan_EventScript_CreateNewProfile:: @ 81B1C36
	msgbox ProfileMan_Text_EvenBetterThanLastProfile, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 0
	call Common_EventScript_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_0x8004, 1
	call_if_eq ProfileMan_EventScript_TryUnlockMysteryEventClub
	compare VAR_RESULT, 0
	goto_if_eq ProfileMan_EventScript_CancelShowProfile
	compare VAR_RESULT, 1
	goto_if_eq ProfileMan_EventScript_ShowProfile
	end

ProfileMan_EventScript_DeclineNewProfile:: @ 81B1C6D
	msgbox ProfileMan_Text_LikeProfileWayItIs, MSGBOX_DEFAULT
	release
	end

ProfileMan_EventScript_TryUnlockMysteryEventClub:: @ 81B1C77
	goto_if_unset FLAG_BADGE05_GET, ProfileMan_EventScript_Return
	goto_if_set FLAG_SYS_EXDATA_ENABLE, ProfileMan_EventScript_Return
	setflag FLAG_SYS_EXDATA_ENABLE
	msgbox ProfileMan_Text_MysteryEventClubUnlocked, MSGBOX_SIGN
	release
	end

ProfileMan_EventScript_Return:: @ 81B1C96
	return
