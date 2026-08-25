LilycoveCity_ContestHall_MapScripts:: @ 8159099
	.byte 0

	.byte 0 @ XXX

LilycoveCity_ContestHall_EventScript_Boy1:: @ 815909B
	msgbox LilycoveCity_ContestHall_Text_TodayWonSmartnessContest, MSGBOX_NPC
	end

LilycoveCity_ContestHall_EventScript_Boy2:: @ 81590A4
	msgbox LilycoveCity_ContestHall_Text_EnteredBunchOfContests, MSGBOX_NPC
	end

LilycoveCity_ContestHall_EventScript_Girl:: @ 81590AD
	msgbox LilycoveCity_ContestHall_Text_ManWhoWonEarlierHadPokeblocks, MSGBOX_NPC
	end

LilycoveCity_ContestHall_EventScript_Sailor:: @ 81590B6
	msgbox LilycoveCity_ContestHall_Text_IsntThisPlaceHumongous, MSGBOX_NPC
	end

LilycoveCity_ContestHall_EventScript_SmartContestMC:: @ 81590BF
	msgbox LilycoveCity_ContestHall_Text_GiveItBestSmartAppeal, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_MC, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestJudge:: @ 81590D4
	msgbox LilycoveCity_ContestHall_Text_AreYouEnjoyingThisContest, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_JUDGE, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestant1:: @ 81590E9
	msgbox LilycoveCity_ContestHall_Text_EnteredWrongContest, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_CONTESTANT_1, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestant2:: @ 81590FE
	msgbox LilycoveCity_ContestHall_Text_RaisedMonToBeSmart, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_CONTESTANT_2, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestant3:: @ 8159113
	msgbox LilycoveCity_ContestHall_Text_IfMonPullsSmartMoveNext, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_CONTESTANT_3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestant4:: @ 8159128
	msgbox LilycoveCity_ContestHall_Text_DontAppreciateCuteLeechLife, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_CONTESTANT_4, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestAudience1:: @ 815913D
	msgbox LilycoveCity_ContestHall_Text_YoureBeautifulGrandpa, MSGBOX_SIGN
	end

LilycoveCity_ContestHall_EventScript_SmartContestAudience2:: @ 8159146
	msgbox LilycoveCity_ContestHall_Text_AllSeemToUseDifferentMoves, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_AUDIENCE_2, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestAudience3:: @ 815915B
	msgbox LilycoveCity_ContestHall_Text_PokemonSmarterThanTrainers, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_AUDIENCE_3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_SmartContestAudience4:: @ 8159170
	msgbox LilycoveCity_ContestHall_Text_StillLoveSmartnessContests, MSGBOX_NPC
	lockall
	applymovement LOCALID_SMART_AUDIENCE_4, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestMC:: @ 8159185
	msgbox LilycoveCity_ContestHall_Text_AreYouEnteringBeautyContest, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_MC, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestJudge:: @ 815919A
	msgbox LilycoveCity_ContestHall_Text_EveryPokemonPristineBeauty, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_JUDGE, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestant1:: @ 81591AF
	msgbox LilycoveCity_ContestHall_Text_EyesWillBeGluedToMyBeauty, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_CONTESTANT_1, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestant2:: @ 81591C4
	msgbox LilycoveCity_ContestHall_Text_OverdidGrooming, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_CONTESTANT_2, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestant3:: @ 81591D9
	msgbox LilycoveCity_ContestHall_Text_JudgeWontSeeAuroraBeam, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_CONTESTANT_3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestant4:: @ 81591EE
	msgbox LilycoveCity_ContestHall_Text_PokemonLooksLikeYoungerMe, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_CONTESTANT_4, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestAudience1:: @ 8159203
	msgbox LilycoveCity_ContestHall_Text_WinBeautyContestMakesMeHappy, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_AUDIENCE_1, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestAudience2:: @ 8159218
	msgbox LilycoveCity_ContestHall_Text_GanderAtAllThosePrettyPokemon, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_AUDIENCE_2, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyContestAudience3:: @ 815922D
	msgbox LilycoveCity_ContestHall_Text_CantWinOnBeautyAlone, MSGBOX_NPC
	lockall
	applymovement LOCALID_BEAUTY_AUDIENCE_3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestMC:: @ 8159242
	msgbox LilycoveCity_ContestHall_Text_InTheMiddleOfContest, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_MC, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestJudge:: @ 8159257
	msgbox LilycoveCity_ContestHall_Text_SuchCharmingCuteAppeals, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_JUDGE, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestant1:: @ 815926C
	msgbox LilycoveCity_ContestHall_Text_MyAzurillWasDistracted, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_CONTESTANT_1, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestant2:: @ 8159281
	msgbox LilycoveCity_ContestHall_Text_NeverWonBattleButContest, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_CONTESTANT_2, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestant3:: @ 8159296
	msgbox LilycoveCity_ContestHall_Text_PetalDanceIsMarvel, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_CONTESTANT_3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestant4:: @ 81592AB
	msgbox LilycoveCity_ContestHall_Text_MyMonAppealSoMuchCuter, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_CONTESTANT_4, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestAudience1:: @ 81592C0
	lockall
	applymovement LOCALID_CUTE_AUDIENCE_1, Common_Movement_FacePlayer
	waitmovement 0
	delay 25
	applymovement LOCALID_CUTE_AUDIENCE_1, Common_Movement_FaceOriginalDirection
	waitmovement 0
	msgbox LilycoveCity_ContestHall_Text_MyChildIsInContest, MSGBOX_SIGN
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestAudience2:: @ 81592E2
	msgbox LilycoveCity_ContestHall_Text_WantCuteMonOfMyOwn, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_AUDIENCE_2, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_CuteContestAudience3:: @ 81592F7
	msgbox LilycoveCity_ContestHall_Text_ThatGirlThereIsCutest, MSGBOX_NPC
	lockall
	applymovement LOCALID_CUTE_AUDIENCE_3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

LilycoveCity_ContestHall_EventScript_BeautyStageSign:: @ 815930C
	msgbox LilycoveCity_ContestHall_Text_BeautyContestStage, MSGBOX_SIGN
	end

LilycoveCity_ContestHall_EventScript_CuteStageSign:: @ 8159315
	msgbox LilycoveCity_ContestHall_Text_CuteContestStage, MSGBOX_SIGN
	end

LilycoveCity_ContestHall_EventScript_SmartStageSign:: @ 815931E
	msgbox LilycoveCity_ContestHall_Text_SmartContestStage, MSGBOX_SIGN
	end
