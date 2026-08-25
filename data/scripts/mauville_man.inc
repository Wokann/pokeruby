#include "constants/mauville_man.h"

MauvilleCity_PokemonCenter_1F_EventScript_MauvilleOldMan:: @ 81AE744
	special Script_GetCurrentMauvilleMan
	switch VAR_RESULT
	case MAUVILLE_MAN_BARD,        MauvilleCity_PokemonCenter_1F_EventScript_Bard
	case MAUVILLE_MAN_HIPSTER,     MauvilleCity_PokemonCenter_1F_EventScript_Hipster
	case MAUVILLE_MAN_TRADER,      MauvilleCity_PokemonCenter_1F_EventScript_Trader
	case MAUVILLE_MAN_STORYTELLER, MauvilleCity_PokemonCenter_1F_EventScript_Storyteller
	case MAUVILLE_MAN_GIDDY,       MauvilleCity_PokemonCenter_1F_EventScript_Giddy
	end


@-------------------------------------------------------------------------------
@ Bard
@-------------------------------------------------------------------------------

MauvilleCity_PokemonCenter_1F_EventScript_Bard::
	lock
	faceplayer
	msgbox MauvilleCity_PokemonCenter_1F_Text_WouldYouLikeToHearMySong, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_PlaySong
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineSong
	end

MauvilleCity_PokemonCenter_1F_EventScript_PlaySong::
	setvar VAR_0x8004, 0
	@ Give the player ear rape
	special PlayBardSong
	delay 60
	special HasBardSongBeenChanged
	compare VAR_RESULT, FALSE
	@ Prompt new lyrics only if song hasn't been changed
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_AskToWriteLyrics
	msgbox MauvilleCity_PokemonCenter_1F_Text_WishICouldPlaySongForOthers, MSGBOX_DEFAULT
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_DeclineSong::
	msgbox MauvilleCity_PokemonCenter_1F_Text_BardFeelingTheBlues1, MSGBOX_DEFAULT
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_AskToWriteLyrics::
	msgbox MauvilleCity_PokemonCenter_1F_Text_WouldYouLikeToWriteSomeLyrics, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_WriteLyrics
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineWritingLyrics
	end

MauvilleCity_PokemonCenter_1F_EventScript_WriteLyrics::
	setvar VAR_0x8004, 6
	call Common_EventScript_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineWritingLyrics
	msgbox MauvilleCity_PokemonCenter_1F_Text_LetMeSingItForYou, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1
	@ Give the player ear rape again
	special PlayBardSong
	delay 60
	msgbox MauvilleCity_PokemonCenter_1F_Text_ThatHowYouWantedSongToGo, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_WriteLyrics		@ Keep looping until player responds YES
	special SaveBardSongLyrics
	msgbox MauvilleCity_PokemonCenter_1F_Text_IllSingThisSongForAWhile, MSGBOX_DEFAULT
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_DeclineWritingLyrics::
	msgbox MauvilleCity_PokemonCenter_1F_Text_BardFeelingTheBlues2, MSGBOX_DEFAULT
	release
	end


@-------------------------------------------------------------------------------
@ Hipster
@-------------------------------------------------------------------------------

MauvilleCity_PokemonCenter_1F_EventScript_Hipster::
	lock
	faceplayer
	setflag FLAG_SYS_HIPSTER_MEET
	msgbox MauvilleCity_PokemonCenter_1F_Text_TeachWhatsHipAndHappening, MSGBOX_DEFAULT
	special HasHipsterTaughtWord
	compare VAR_RESULT, FALSE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_TryTeachWord
	msgbox MauvilleCity_PokemonCenter_1F_Text_IAlreadyTaughtYou, MSGBOX_DEFAULT
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_TryTeachWord::
	special HipsterTryTeachWord
	compare VAR_RESULT, TRUE		@ TRUE if player learned a new word
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_TeachWord
	msgbox MauvilleCity_PokemonCenter_1F_Text_IveGotNothingNewToTeach, MSGBOX_DEFAULT
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_TeachWord::
	msgbox MauvilleCity_PokemonCenter_1F_Text_HaveYouHeardOfWord, MSGBOX_DEFAULT
	special SetHipsterTaughtWord
	release
	end


@-------------------------------------------------------------------------------
@ Trader
@-------------------------------------------------------------------------------

	.include "data/text/trader.inc"

MauvilleCity_PokemonCenter_1F_EventScript_Trader::
	lock
	faceplayer
	msgbox MauvilleCity_PokemonCenter_1F_Text_WantToTradeDecor, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineTrade
	special ScrSpecial_GetTraderTradedFlag
	compare VAR_RESULT, TRUE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_AlreadyTraded
	message MauvilleCity_PokemonCenter_1F_Text_PickADecorItem
	waitmessage
	goto MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToReceive
	end

MauvilleCity_PokemonCenter_1F_EventScript_DeclineTrade::
	msgbox MauvilleCity_PokemonCenter_1F_Text_TraderFeelingTheBlues, MSGBOX_NPC
	end

MauvilleCity_PokemonCenter_1F_EventScript_AlreadyTraded::
	msgbox MauvilleCity_PokemonCenter_1F_Text_WeveAlreadyTraded, MSGBOX_NPC
	end

MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToReceive::
	special ScrSpecial_TraderMenuGetDecoration
	waitstate
	compare VAR_0x8004, 0
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_CancelPickDecor
	compare VAR_0x8004, -1
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_InvalidDecor
	msgbox MauvilleCity_PokemonCenter_1F_Text_OnceBelongedToPlayerDoYouWantIt, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_PickDifferentDecor
	special ScrSpecial_DoesPlayerHaveNoDecorations
	compare VAR_RESULT, TRUE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DontHaveAnyDecor
	goto MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToGive
	end

MauvilleCity_PokemonCenter_1F_EventScript_CancelPickDecor::
	msgbox MauvilleCity_PokemonCenter_1F_Text_YouDontWantAnything, MSGBOX_NPC
	end

MauvilleCity_PokemonCenter_1F_EventScript_InvalidDecor::
	message MauvilleCity_PokemonCenter_1F_Text_CantTradeThatOne
	waitmessage
	goto MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToReceive
	end

MauvilleCity_PokemonCenter_1F_EventScript_PickDifferentDecor::
	message MauvilleCity_PokemonCenter_1F_Text_PickADecorItem
	waitmessage
	goto MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToReceive
	end

MauvilleCity_PokemonCenter_1F_EventScript_DontHaveAnyDecor::
	msgbox MauvilleCity_PokemonCenter_1F_Text_YouDontHaveAnyDecor, MSGBOX_NPC
	end

MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToGive::
	msgbox MauvilleCity_PokemonCenter_1F_Text_PickTheDecorToTrade, MSGBOX_DEFAULT
	special ScrSpecial_TraderMenuGiveDecoration
	waitstate
	compare VAR_0x8006, 0
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_CancelGiveDecor
	compare VAR_0x8006, -1
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DecorInUse
	special ScrSpecial_IsDecorationFull
	compare VAR_RESULT, 1
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_NoRoomForDecor
	msgbox MauvilleCity_PokemonCenter_1F_Text_SoWellTradeTheseDecor, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToGive
	special ScrSpecial_TraderDoDecorationTrade
	msgbox MauvilleCity_PokemonCenter_1F_Text_SendDecorToYourPC, MSGBOX_NPC
	end

MauvilleCity_PokemonCenter_1F_EventScript_CancelGiveDecor::
	msgbox MauvilleCity_PokemonCenter_1F_Text_YouDontWantToTrade, MSGBOX_NPC
	end

MauvilleCity_PokemonCenter_1F_EventScript_DecorInUse::
	msgbox MauvilleCity_PokemonCenter_1F_Text_ThatDecorIsInUse, MSGBOX_DEFAULT
	goto MauvilleCity_PokemonCenter_1F_EventScript_PickDecorToGive
	end

MauvilleCity_PokemonCenter_1F_EventScript_NoRoomForDecor::
	msgbox MauvilleCity_PokemonCenter_1F_Text_YouveNoRoomForThis, MSGBOX_NPC
	end


@-------------------------------------------------------------------------------
@ Storyteller
@-------------------------------------------------------------------------------

	.include "data/text/storyteller.inc"

MauvilleCity_PokemonCenter_1F_EventScript_Storyteller::
	setvar VAR_0x8008, 0
	setvar VAR_0x8009, 0
	setvar VAR_0x800A, 0
	setvar VAR_0x800B, 0
	lock
	faceplayer
	msgbox MauvilleCity_PokemonCenter_1F_Text_WillYouHearMyTale, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineStoryteller
	specialvar VAR_RESULT, StorytellerGetFreeStorySlot
	compare VAR_RESULT, 0		@ If slot is 0, then the list is empty
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_KnowNoTales
MauvilleCity_PokemonCenter_1F_EventScript_ChooseStory::
	message MauvilleCity_PokemonCenter_1F_Text_WhichTaleToTell
	waitmessage
	special StorytellerStoryListMenu
	waitstate
	compare VAR_RESULT, FALSE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_CancelStorySelection
	setvar VAR_0x8008, 1
	special Script_StorytellerDisplayStory
	waitmessage
	waitbuttonpress
	specialvar VAR_RESULT, StorytellerUpdateStat
	compare VAR_RESULT, FALSE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_AskToHearAnotherTale
	goto MauvilleCity_PokemonCenter_1F_EventScript_TellPlayersTale
MauvilleCity_PokemonCenter_1F_EventScript_CancelStorySelection::
	compare VAR_0x8008, 0
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineStoryteller
	goto MauvilleCity_PokemonCenter_1F_EventScript_TryRecordStory
MauvilleCity_PokemonCenter_1F_EventScript_AskToHearAnotherTale::
	msgbox MauvilleCity_PokemonCenter_1F_Text_CouldThereBeOtherLegends, MSGBOX_DEFAULT
	msgbox MauvilleCity_PokemonCenter_1F_Text_HearAnotherLegendaryTale, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_ChooseStory
MauvilleCity_PokemonCenter_1F_EventScript_TryRecordStory::
	specialvar VAR_RESULT, HasStorytellerAlreadyRecorded
	compare VAR_RESULT, TRUE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_StorytellerNoMoreTales		@ already recorded story
	specialvar VAR_RESULT, StorytellerGetFreeStorySlot
	compare VAR_RESULT, 4
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_StorytellerNoMoreTales		@ story list is full
	goto MauvilleCity_PokemonCenter_1F_EventScript_DoYouHaveAnyTales
MauvilleCity_PokemonCenter_1F_EventScript_KnowNoTales::
	msgbox MauvilleCity_PokemonCenter_1F_Text_IKnowNoTales, MSGBOX_DEFAULT
MauvilleCity_PokemonCenter_1F_EventScript_DoYouHaveAnyTales::
	msgbox MauvilleCity_PokemonCenter_1F_Text_HaveYouAnyLegendaryTales, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineStoryteller
	specialvar VAR_RESULT, Script_StorytellerInitializeRandomStat
	compare VAR_RESULT, TRUE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_TellPlayersTale
	msgbox MauvilleCity_PokemonCenter_1F_Text_NotWorthyOfLegend, MSGBOX_DEFAULT
	closemessage
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_TellPlayersTale::
	msgbox MauvilleCity_PokemonCenter_1F_Text_YouDidStatXTimes, MSGBOX_DEFAULT
	closemessage
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_DeclineStoryteller::
	msgbox MauvilleCity_PokemonCenter_1F_Text_StorytellerFeelingTheBlues, MSGBOX_DEFAULT
	closemessage
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_StorytellerNoMoreTales::
	msgbox MauvilleCity_PokemonCenter_1F_Text_IWishMorePeopleWereInterested, MSGBOX_DEFAULT
	closemessage
	release
	end


@-------------------------------------------------------------------------------
@ Giddy
@-------------------------------------------------------------------------------

	.include "data/text/giddy.inc"

MauvilleCity_PokemonCenter_1F_EventScript_Giddy::
	lock
	faceplayer
	msgbox MauvilleCity_PokemonCenter_1F_Text_HearMyStory, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_TryTellTale
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_DeclineGiddy
	end

MauvilleCity_PokemonCenter_1F_EventScript_TryTellTale::
	special GiddyShouldTellAnotherTale
	compare VAR_RESULT, TRUE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_GiddyTellTale
	compare VAR_RESULT, FALSE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_ToldEnoughTales
	end

MauvilleCity_PokemonCenter_1F_EventScript_TryTellNewTale::
	special GiddyShouldTellAnotherTale
	compare VAR_RESULT, TRUE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_GiddyStartNewTale
	compare VAR_RESULT, FALSE
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_ToldEnoughTales
	end

MauvilleCity_PokemonCenter_1F_EventScript_GiddyStartNewTale::
	msgbox MauvilleCity_PokemonCenter_1F_Text_AlsoIWasThinking, MSGBOX_DEFAULT
	goto MauvilleCity_PokemonCenter_1F_EventScript_GiddyTellTale
	end

MauvilleCity_PokemonCenter_1F_EventScript_GiddyTellTale::
	special GenerateGiddyLine
	special ShowFieldMessageStringVar4
	waitmessage
	yesnobox 20, 8
	compare VAR_RESULT, 1
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_TryTellNewTale
	compare VAR_RESULT, 0
	goto_if_eq MauvilleCity_PokemonCenter_1F_EventScript_TryTellNewTale
	end

MauvilleCity_PokemonCenter_1F_EventScript_DeclineGiddy::
	msgbox MauvilleCity_PokemonCenter_1F_Text_GiddyFeelingTheBlues, MSGBOX_DEFAULT
	release
	end

MauvilleCity_PokemonCenter_1F_EventScript_ToldEnoughTales::
	msgbox MauvilleCity_PokemonCenter_1F_Text_WeShouldChatAgain, MSGBOX_DEFAULT
	release
	end

	.include "data/text/bard.inc"
	.include "data/text/hipster.inc"
