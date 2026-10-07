	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	@ unused
gOtherText_TopSpeed::
	.string "TOP SPEED$"

gText_BlenderMaxSpeedRecord:: @ 842C2E6
	.string "BERRY BLENDER\nMAXIMUM SPEED RECORD!$"

gText_234Players:: @ 842C30A
	.string "2 PLAYERS\n3 PLAYERS\n4 PLAYERS$"

gMultiText_Saving:: @ 842C328
	.string "SAVING...\nDON'T TURN OFF THE POWER.$"

gMultiText_LinkError:: @ 842C34C
	.string "{HIGHLIGHT TRANSPARENT}{COLOR WHITE2}Link error...\nCheck your Game Link cable connection,\nand turn the power off and on.$"

gText_Option:: @ 842C3A6
	.string "{PALETTE 9}OPTION$"

gText_TextSpeed:: @ 842C3B0
	.string "{PALETTE 9}TEXT SPEED$"

gText_BattleScene:: @ 842C3BE
	.string "{PALETTE 9}BATTLE SCENE$"

gText_BattleStyle:: @ 842C3CE
	.string "{PALETTE 9}BATTLE STYLE$"

gText_Sound:: @ 842C3DE
	.string "{PALETTE 9}SOUND$"

gText_Frame:: @ 842C3E7
	.string "{PALETTE 9}FRAME$"

gText_OptionMenuCancel:: @ 842C3F0
	.string "{PALETTE 9}CANCEL$"

gText_ButtonMode:: @ 842C3FA
	.string "{PALETTE 9}BUTTON MODE$"

gText_TextSpeedSlow:: @ 842C409
	.string "{PALETTE 15}SLOW$"

gText_TextSpeedMid:: @ 842C411
	.string "{PALETTE 15}MID$"

gText_TextSpeedFast:: @ 842C418
	.string "{PALETTE 15}FAST$"

gText_BattleSceneOn:: @ 842C420
	.string "{PALETTE 15}ON$"

gText_BattleSceneOff:: @ 842C426
	.string "{PALETTE 15}OFF$"

gText_BattleStyleShift:: @ 842C42D
	.string "{PALETTE 15}SHIFT$"

gText_BattleStyleSet:: @ 842C436
	.string "{PALETTE 15}SET$"

gText_SoundMono:: @ 842C43D
	.string "{PALETTE 15}MONO$"

gText_SoundStereo:: @ 842C445
	.string "{PALETTE 15}STEREO$"

gText_FrameType:: @ 842C44F
	.string "{PALETTE 15}TYPE$"

gText_FrameTypeNumber:: @ 842C457
	.string "{PALETTE 8}$"

gText_ButtonTypeNormal:: @ 842C45B
	.string "{PALETTE 15}NORMAL$"

gText_ButtonTypeLR:: @ 842C465
	.string "{PALETTE 15}LR$"

gText_ButtonTypeLEqualsA:: @ 842C46B
	.string "{PALETTE 15}L=A$"

gText_MenuPokedex::
	.string "POKéDEX$"
gText_MenuPokemon::
	.string "POKéMON$"
gText_MenuBag::
	.string "BAG$"
gText_MenuPokenav::
	.string "POKéNAV$"
gText_MenuPlayer::
	.string "{PLAYER}$"
gText_MenuSave::
	.string "SAVE$"
gText_MenuOption::
	.string "OPTION$"
gText_MenuExit::
	.string "EXIT$"
gText_MenuRetire:: @ safari zone?
	.string "RETIRE$"

gText_SafariBallStock:: @ 842C4A9
	.string "SAFARI BALLS\nStock: {STR_VAR_1}$"

gText_LinkStandby2:: @ 842C4C0
	.string "Link standby...\n... ... B Button: Cancel$"

gText_PressAToLoadEvent:: @ 842C4E9
	.string "Press the A Button to load event.\n... ... B Button: Cancel$"

gText_LoadingEvent:: @ 842C524
	.string "Loading event...$"

gText_DontRemoveCableTurnOff:: @ 842C535
	.string "Don't remove the Game Link cable.\nDon't turn off the power.$"

gText_EventSafelyLoaded:: @ 842C571
	.string "The event was safely loaded.$$"

gText_LoadErrorEndingSession:: @ 842C58F
	.string "Loading error.\nEnding session.$$$$$$$$"

gText_Hoenn:: @ 842C5B5
	.string "HOENN$"

gText_OhABite:: @ 842C5BB
	.string "Oh! A bite!$"

gText_PokemonOnHook:: @ 842C5C7
	.string "A POKéMON's on the hook!{PAUSE_UNTIL_PRESS}$"

gText_NotEvenANibble:: @ 842C5E2
	.string "Not even a nibble...{PAUSE_UNTIL_PRESS}$"

gText_ItGotAway:: @ 842C5F9
	.string "It got away...{PAUSE_UNTIL_PRESS}$$"

gText_XWillBeSentToY:: @ 842C60B
	.string "{STR_VAR_2} will be sent to {STR_VAR_1}.$"

gText_ByeByeVar1:: @ 842C622
	.string "Bye-bye, {STR_VAR_2}!$"

gText_XSentOverY:: @ 842C62F
	.string "{STR_VAR_1} sent over {STR_VAR_3}.$"

gText_TakeGoodCareOfX:: @ 842C640
	.string "Take good care of {STR_VAR_3}!$"

sText_Cancel::
	.string "CANCEL$"
sText_ChooseAPkmn::
	.string "Choose a POKéMON.$"
sText_Summary::
	.string "SUMMARY$"
sText_Trade::
	.string "TRADE$"
sText_CancelTrade::
	.string "Cancel trade?$"
TradeText_PressBToExit::
	.string "Press the B Button to exit.$"
sText_Summary2::
	.string "SUMMARY$"
sText_Trade2::
	.string "TRADE$"
sText_CommunicationStandby::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Link standby...\nPlease wait.$"
sText_TheTradeHasBeenCanceled::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}The trade has\nbeen canceled.$"
sText_OnlyPkmnForBattle::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}That's your only\nPOKéMON for battle.$"
TradeText_NonTradablePoke::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Your friend can't\ntrade that one.$"
sText_WaitingForYourFriend::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Waiting for your\nfriend to finish...$"
sText_YourFriendWantsToTrade::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Your friend wants\nto trade POKéMON.$"
	.string "$"

sText_IsThisTradeOkay:: @ 842C7B6
	.string "Is this trade okay?$"

gOtherText_Terminator2:: @ 842C7CA
	.string "$"

gOtherText_ControlAndMiscText:: @ 842C7CB
	.string "\n$"
	.string "/$"
	.string "Lv$"
	.string "---$"

gOtherText_FourQuestions:: @ 842C7D6
	.string "????$"

@ 842C7DB
	.include "data/text/easy_chat/group_name_strings.inc"

gText_ThreeQuestionMarks:: @ 842C890
	.string "???$"

gOtherText_From:: @ 842C894
	.string "From $"

gText_MixingRecords:: @ 842C89A
	.string "Mixing records...$"

gText_RecordMixingComplete:: @ 842C8AC
	.string "Record mixing completed.\nThank you for waiting.$"

gOtherText_TrainersTrainerCard:: @ 842C8DC
	.string "'s TRAINER CARD$"

gText_HallOfFameDebut:: @ 842C8EC
	.string "FIRST HALL OF FAME$"

gText_LinkCableBattles:: @ 842C8FF
	.string "LINK CABLE BATTLES  W/            L/$"

gText_BattleTower:: @ 842C924
	.string "BATTLE TOWER W/{CLEAR_TO 0x72}&{CLEAR_TO 0x98}STRAIGHT$"

gText_WonContestsWFriends:: @ 842C943
	.string "CONTESTS W/FRIENDS W/$"

gText_PokeblocksWithFriends:: @ 842C959
	.string "{POKEBLOCK}S W/FRIENDS$"

gText_PokemonTrades:: @ 842C96A
	.string "POKéMON TRADES$"

gOtherText_Boy:: @ 842C979
	.string "BOY$"

gOtherText_Girl:: @ 842C97D
	.string "GIRL$"
	.string "$"

gText_SavingPlayer:: @ 842C983
	.string "PLAYER$"

gText_SavingBadges:: @ 842C98A
	.string "BADGES$"
	.string "$"

gText_SavingPokedex:: @ 842C992
	.string "POKéDEX$"

gText_SavingTime:: @ 842C99A
	.string "PLAY TIME$"

gOtherText_SentToPC:: @ 842C9A4
	.string "{STR_VAR_1} was sent to\nthe PC.{PAUSE_UNTIL_PRESS}$"

OtherText_YourName::
	.string "YOUR NAME?$"
OtherText_BoxName::
	.string "BOX NAME?$"
OtherText_PokeName::
	.string "{STR_VAR_1}'s nickname?$"

gText_Marco:: @ 842C9E1
	.string "MARCO$" @ polo

gOtherText_Coins:: @ 842C9E7
	.string "{STR_VAR_1} COINS$"

gText_PlayersBattleResults:: @ 842C9F0
	.string "{PLAYER}'s BATTLE RESULTS$"

gText_TotalRecordWLD:: @ 842CA04
	.string "TOTAL RECORD W/{STR_VAR_1} L/{STR_VAR_2} D/{STR_VAR_3}$"

gText_WinLoseDraw:: @ 842CA20
	.string "WIN        LOSE        DRAW$"

gOtherText_SevenDashes:: @ 842CA3C
	.string "-------$"

gOtherText_FourDashes:: @ 842CA44
	.string "----$"

gOtherText_BattleTowerResults:: @ 842CA49
	.string "{PLAYER}'s BATTLE TOWER RESULTS$"

gText_Lv502:: @ 842CA63
	.string "LV 50$"

gOtherText_Lv100:: @ 842CA69
	.string "LV 100$"

gText_WinStreak:: @ 842CA70
	.string "{STR_VAR_1}W STREAK$"

gText_Current:: @ 842CA7B
	.string "CURRENT$"

gText_Record:: @ 842CA83
	.string "RECORD$"

gText_Prev:: @ 842CA8A
	.string "PREV.$"

gOtherText_QuitGamePrompt:: @ 842CA90
	.string "Quit the game?$"

gOtherText_MaxCoins:: @ 842CA9F
	.string "You've got 9,999 COINS.$"

gOtherText_OutOfCoins:: @ 842CAB7
	.string "You've run out of COINS.\nGame over!$"

gOtherText_DontHaveThreeCoins:: @ 842CADB
	.string "You don't have three COINS.$"

gOtherText_ReelTime:: @ 842CAF7
	.string "REEL TIME\nHere's your chance to take\naim and nail marks!\nThe Reel Time continues for\nas often as the drawn number.\nIt all ends on a Big Bonus.$"

gDaycareText_GetAlongVeryWell::
	.string "The two seem to get along very well.$"
gDaycareText_GetAlong::
	.string "The two seem to get along.$"
gDaycareText_DontLikeOther::
	.string "The two don't seem to like each other.$"
gDaycareText_PlayOther::
	.string "The two prefer to play with other\nPOKéMON than each other.$"

gOtherText_NewLine2:: @ 842CC28
	.string "\n$"

gOtherText_CancelAndLv:: @ 842CC2A
	.string "CANCEL$"
	.string "LV$"

gEggNickname:: @ 842CC34
	.string "EGG$"

gOtherText_HatchedFromEgg:: @ 842CC38
	.string "{COLOR WHITE2}{HIGHLIGHT TRANSPARENT}{SHADOW DARK_GREY}{STR_VAR_1} hatched from the EGG!$"

gOtherText_NickHatchPrompt:: @ 842CC5A
	.string "{COLOR WHITE2}{HIGHLIGHT TRANSPARENT}{SHADOW DARK_GREY}Would you like to nickname the newly\nhatched {STR_VAR_1}?$"

gText_MoveRelearnerBattleMoves::
	.string "BATTLE$"
gText_MoveRelearnerContestMovesTitle::
	.string "CONTEST$"
OtherText_Type::
	.string "TYPE/$"
OtherText_PP::
	.string "PP/$"
gText_MoveRelearnerPower::
	.string "POWER$"
gText_MoveRelearnerAccuracy::
	.string "ACCURACY$"
gText_MoveRelearnerAppeal::
	.string "APPEAL$"
gText_MoveRelearnerJam::
	.string "JAM$"

gText_TeachWhichMoveToPkmn:: @ 842CCC7
	.string "Teach which move to {STR_VAR_1}?$"

gText_MoveRelearnerTeachMoveConfirm:: @ 842CCDF
	.string "Teach {STR_VAR_2}?$"

gText_MoveRelearnerPkmnLearnedMove:: @ 842CCE9
	.string "{STR_VAR_1} learned {STR_VAR_2}.$"

gText_MoveRelearnerPkmnTryingToLearnMove:: @ 842CCF8
	.string "{STR_VAR_1} is trying to learn\n{STR_VAR_2}.\pBut {STR_VAR_1} can't learn more\nthan four moves.\pDelete an older move to make\nroom for {STR_VAR_2}?$"

gText_MoveRelearnerStopTryingToTeachMove:: @ 842CD65
	.string "Stop learning {STR_VAR_2}?$"

gText_MoveRelearnerAndPoof:: @ 842CD77
	.string "{PAUSE 32}1, {PAUSE 15}2 {PAUSE 15}...{PAUSE 15}...{PAUSE 15}...{PAUSE 15} {PLAY_SE 0x0038}Poof!\p$"

gOtherText_ForgotOrDidNotLearnMove:: @ 842CDA3
	.string "{STR_VAR_1} forgot {STR_VAR_3}.\pAnd...\p{STR_VAR_1} learned {STR_VAR_2}.$"
	.string "{STR_VAR_1} did not learn\n{STR_VAR_2}.$"

gText_MoveRelearnerGiveUp:: @ 842CDDC
	.string "Give up trying to teach a new move\nto {STR_VAR_1}?$"

gText_MoveRelearnerWhichMoveToForget:: @ 842CE06
	.string "Which move should be forgotten?\p$"

gSystemText_ClearAllSaveDataPrompt:: @ 842CE27
	.string "Clear all save data areas?$"

gSystemText_ClearingData:: @ 842CE42
	.string "Clearing data...\nPlease wait.$"

gText_AnnouncingResults:: @ 842CE60
	.string "Announcing the results!$"

gText_PreliminaryResults:: @ 842CE78
	.string "The preliminary results!$"

gText_Round2Results:: @ 842CE91
	.string "Round 2 results!$"

gText_ContestantsMonWon:: @ 842CEA2
	.string "{STR_VAR_1}'s {STR_VAR_2} won!$"

gText_CommunicationStandby:: @ 842CEAF
	.string "Link standby...$"

@ 842CEBF
	.include "data/text/landmarks.inc"

gOtherText_Terminator4:: @ 842D0E6
	.string "$"

gOtherText_Nature:: @ 842D0E7
	.string " nature, $"

gOtherText_Comma:: @ 842D0F1
	.string ",$"

gOtherText_Met:: @ 842D0F3
	.string " (met).$"

gOtherText_Egg2:: @ 842D0FB
	.string " (EGG).$"

gOtherText_ObtainedInTrade:: @ 842D103
	.string "obtained in a trade.$"

gOtherText_FatefulEncounter:: @ 842D118
	.string "fateful encounter.$"

gOtherText_Met2:: @ 842D12B
	.string " (met).$"

gOtherText_EggDayCare:: @ 842D133
	.string "An odd POKéMON EGG found\nby the DAY CARE couple.$"

gOtherText_EggNicePlace:: @ 842D164
	.string "A peculiar POKéMON EGG\nobtained at the nice place.$"

gOtherText_EggObtainedInTrade:: @ 842D197
	.string "A peculiar POKéMON EGG\nobtained in a trade.$"

gOtherText_EggHotSprings:: @ 842D1C3
	.string "A POKéMON EGG obtained\nat the hot springs.$"

gText_CommunicationStandby5:: @ 842D1EE
	.string "Link standby...$"

gText_YesNo:: @ 842D1FE
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}YES\nNO$"
	.string "{HIGHLIGHT TRANSPARENT}PLAYER$"
	.string "{HIGHLIGHT TRANSPARENT}$"

gText_DexHoenn:: @ 842D21C
	.string "HOENN$"

gText_DexNational:: @ 842D222
	.string "NATIONAL$"

gText_PokedexDiploma:: @ 842D22B
	.string "{HIGHLIGHT TRANSPARENT}Player: {CLEAR 0x10}{COLOR RED}{PLAYER}\n\n{COLOR DARK_GREY}{STR_VAR_1} POKéDEX\nThis document certifies\nthat you have successfully\ncompleted your POKéDEX.\n\n{CLEAR_TO 0x42}{COLOR RED}GAME FREAK$"
	.string "{COLOR RED}{HIGHLIGHT TRANSPARENT}GAME FREAK$"

gText_NumPlayerLink:: @ 842D2BC
	.string "{STR_VAR_1}P LINK$"

gOtherText_YesNoTerminating:: @ 842D2C5
	.string "YES\nNO$"
	.string "Terminating.$"

gOtherText_LinkNotFound:: @ 842D2D9
	.string "Link partner(s) not found.\nPlease try again.\p$"

sText_BerryBlenderStart:: @ 842D307
	.string "Starting up the BERRY BLENDER.\pPlease select a BERRY from your BAG\nto put in the BERRY BLENDER.\p$"

sText_WasMade:: @ 842D368
	.string " was made!$"

sText_PressAToStart:: @ 842D373
	.string "Press the A Button to start.$"

sText_PleaseWaitAWhile:: @ 842D390
	.string "Please wait a while.$"

gOtherText_LinkStandby3:: @ 842D3A5
	.string "Link standby...$"

sText_WouldLikeToBlendAnotherBerry:: @ 842D3B5
	.string "Would you like to blend another BERRY?$"

sText_RunOutOfBerriesForBlending:: @ 842D3DC
	.string "You've run out of BERRIES for\nblending in the BERRY BLENDER.\p$"

sText_YourPokeblockCaseIsFull:: @ 842D41A
	.string "Your {POKEBLOCK} CASE is full.\p$"

sText_HasNoBerriesToPut:: @ 842D434
	.string " has no BERRIES to put in\nthe BERRY BLENDER.$"

sText_ApostropheSPokeblockCaseIsFull:: @ 842D461
	.string "'s {POKEBLOCK} CASE is full.\p$"

sText_BlendingResults:: @ 842D479
	.string "RESULTS OF BLENDING$"
	.string "BERRY USED$"

sText_SpaceBerry:: @ 842D498
	.string " BERRY$"

sText_Time:: @ 842D49F
	.string "REQUIRED TIME$"

sText_Min:: @ 842D4AD
	.string "MIN.$"

sText_Sec:: @ 842D4B2
	.string "SEC.$"

sText_MaximumSpeed:: @ 842D4B7
	.string "MAXIMUM SPEED$"

sText_RPM:: @ 842D4C5
	.string "RPM$"
	.string ".$"

sText_Ranking:: @ 842D4CB
	.string "RANKING$"

sText_TheLevelIs:: @ 842D4D3
	.string "The level is $"

sText_TheFeelIs:: @ 842D4E1
	.string ", and the feel is $"

sText_Dot2:: @ 842D4F4
	.string ".$"

gText_Day:: @ 842D4F6
	.string "DAY$"
	.string ":$"

gOtherText_OK:: @ 842D4FC
	.string "OK$"

gText_IsThisTheCorrectTime:: @ 842D4FF
	.string "Is this the correct time?$"
