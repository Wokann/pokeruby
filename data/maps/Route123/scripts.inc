Route123_MapScripts:: @ 8151C53
	map_script MAP_SCRIPT_ON_TRANSITION, Route123_OnTransition
	.byte 0

Route123_OnTransition:: @ 8151C59
	special SetRoute123Weather
	end

Route123_EventScript_GigaDrainGirl:: @ 8151C5D
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM19, Route123_EventScript_ReceivedGigaDrain
	msgbox Route123_Text_LoveGrassMonsHaveAny, MSGBOX_DEFAULT
	special IsGrassTypeInParty
	compare VAR_RESULT, 0
	goto_if_eq Route123_EventScript_NoGrassMons
	msgbox Route123_Text_YouLikeGrassMonsTooHaveThis, MSGBOX_DEFAULT
	giveitem ITEM_TM19_GIGA_DRAIN
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM19
	msgbox Route123_Text_CheckTreesWithMyGrassMon, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_NoGrassMons:: @ 8151CAA
	release
	end

Route123_EventScript_ReceivedGigaDrain:: @ 8151CAC
	msgbox Route123_Text_CheckTreesWithMyGrassMon, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_RouteSign:: @ 8151CB6
	msgbox Route123_Text_RouteSign, MSGBOX_SIGN
	end

Route123_EventScript_RouteSignMtPyre:: @ 8151CBF
	msgbox Route123_Text_RouteSignMtPyre, MSGBOX_SIGN
	end

Route123_EventScript_BerryMastersHouseSign:: @ 8151CC8
	msgbox Route123_Text_BerryMastersHouse, MSGBOX_SIGN
	end

Route123_EventScript_Wendy:: @ 8151CD1
	trainerbattle_single TRAINER_WENDY, Route123_Text_WendyIntro, Route123_Text_WendyDefeat
	msgbox Route123_Text_WendyPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Clyde:: @ 8151CE8
	trainerbattle_single TRAINER_CLYDE, Route123_Text_ClydeIntro, Route123_Text_ClydeDefeat
	msgbox Route123_Text_ClydePostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Violet:: @ 8151CFF
	trainerbattle_single TRAINER_VIOLET, Route123_Text_VioletIntro, Route123_Text_VioletDefeat
	msgbox Route123_Text_VioletPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Cameron:: @ 8151D16
	trainerbattle_single TRAINER_CAMERON_1, Route123_Text_CameronIntro, Route123_Text_CameronDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route123_EventScript_RematchCameron
	msgbox Route123_Text_CameronPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_RematchCameron:: @ 8151D3D
	trainerbattle_rematch TRAINER_CAMERON_1, Route123_Text_CameronRematchIntro, Route123_Text_CameronRematchDefeat
	msgbox Route123_Text_CameronPostRematch, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Jacki:: @ 8151D54
	trainerbattle_single TRAINER_JACKI_1, Route123_Text_JackiIntro, Route123_Text_JackiDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route123_EventScript_RematchJacki
	msgbox Route123_Text_JackiPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_RematchJacki:: @ 8151D7B
	trainerbattle_rematch TRAINER_JACKI_1, Route123_Text_JackiRematchIntro, Route123_Text_JackiRematchDefeat
	msgbox Route123_Text_JackiPostRematch, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Miu:: @ 8151D92
	trainerbattle_double TRAINER_MIU_AND_YUKI, Route123_Text_MiuIntro, Route123_Text_MiuDefeat, Route123_Text_MiuNotEnoughMons
	msgbox Route123_Text_MiuPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Yuki:: @ 8151DAD
	trainerbattle_double TRAINER_MIU_AND_YUKI, Route123_Text_YukiIntro, Route123_Text_YukiDefeat, Route123_Text_YukiNotEnoughMons
	msgbox Route123_Text_YukiPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Kindra:: @ 8151DC8
	trainerbattle_single TRAINER_KINDRA, Route123_Text_KindraIntro, Route123_Text_KindraDefeat
	msgbox Route123_Text_KindraPostBattle, MSGBOX_AUTOCLOSE
	end
