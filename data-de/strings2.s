	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	@ unused
gOtherText_TopSpeed::
	.string "TEMPO$"

gText_BlenderMaxSpeedRecord:: @ 842C2E6
	.string "BEERENMIXER\n"
	.string "MAXIMALTEMPO-REKORD!$"

gText_234Players:: @ 842C30A
	.string "2 SPIELER\n"
	.string "3 SPIELER\n"
	.string "4 SPIELER$"

gMultiText_Saving:: @ 842C328
	.string "SPEICHERVORGANG...\n"
	.string "DAS GERÄT NICHT AUSSCHALTEN!$"

gMultiText_LinkError:: @ 842C34C
	.string "{HIGHLIGHT TRANSPARENT}{COLOR WHITE2}Verbindungsfehler...\n"
	.string "Die Kabelverbindungen überprüfen und\n"
	.string "die Geräte aus- und wieder einschalten.$"

gText_Option:: @ 842C3A6
	.string "{PALETTE 9}OPTIONEN$"

gText_TextSpeed:: @ 842C3B0
	.string "{PALETTE 9}TEXT-TEMPO$"

gText_BattleScene:: @ 842C3BE
	.string "{PALETTE 9}ANIMATION$"

gText_BattleStyle:: @ 842C3CE
	.string "{PALETTE 9}KAMPFSTIL$"

gText_Sound:: @ 842C3DE
	.string "{PALETTE 9}TON$"

gText_Frame:: @ 842C3E7
	.string "{PALETTE 9}RAHMEN$"

gText_OptionMenuCancel:: @ 842C3F0
	.string "{PALETTE 9}ZURÜCK$"

gText_ButtonMode:: @ 842C3FA
	.string "{PALETTE 9}KNOPFBELEGUNG$"

gText_TextSpeedSlow:: @ 842C409
	.string "{PALETTE 15}1$"

gText_TextSpeedMid:: @ 842C411
	.string "{PALETTE 15}2$"

gText_TextSpeedFast:: @ 842C418
	.string "{PALETTE 15}3$"

gText_BattleSceneOn:: @ 842C420
	.string "{PALETTE 15}EIN$"

gText_BattleSceneOff:: @ 842C426
	.string "{PALETTE 15}AUS$"

gText_BattleStyleShift:: @ 842C42D
	.string "{PALETTE 15}WECHSEL$"

gText_BattleStyleSet:: @ 842C436
	.string "{PALETTE 15}FOLGE$"

gText_SoundMono:: @ 842C43D
	.string "{PALETTE 15}MONO$"

gText_SoundStereo:: @ 842C445
	.string "{PALETTE 15}STEREO$"

gText_FrameType:: @ 842C44F
	.string "{PALETTE 15}MOTIV $"

gText_FrameTypeNumber:: @ 842C457
	.string "{PALETTE 8}$"

gText_ButtonTypeNormal:: @ 842C45B
	.string "{PALETTE 15}NORMAL$"

gText_ButtonTypeLR:: @ 842C465
	.string "{PALETTE 15}L/R$"

gText_ButtonTypeLEqualsA:: @ 842C46B
	.string "{PALETTE 15}L=A$"

gText_MenuPokedex::
	.string "POKéDEX$"
gText_MenuPokemon::
	.string "POKéMON$"
gText_MenuBag::
	.string "BEUTEL$"
gText_MenuPokenav::
	.string "POKéNAV$"
gText_MenuPlayer::
	.string "{PLAYER}$"
gText_MenuSave::
	.string "SICHERN$"
gText_MenuOption::
	.string "OPTION.$"
gText_MenuExit::
	.string "BEENDEN$"
gText_MenuRetire:: @ safari zone?
	.string "SCHLUSS$"

gText_SafariBallStock:: @ 842C4A9
	.string "SAFARIBÄLLE\n"
	.string "Vorrat: {STR_VAR_1}$"

gSystemText_LinkStandby:: @ 842C4C0
	.string "Verbindung...\n"
	.string "... ... B-Knopf: Abbrechen$"

gSystemText_LoadEventPressA:: @ 842C4E9
	.string "Lade Geschehen mit A-Knopf.\n"
	.string "... ... B-Knopf: Abbrechen$"

gSystemText_LoadingEvent:: @ 842C524
	.string "Geschehen wird geladen.$"

gSystemText_DontCutLink:: @ 842C535
	.string "Verbindungskabel nicht entfernen!\n"
	.string "Die Geräte nicht ausschalten!$"

gSystemText_EventLoadSuccess:: @ 842C571
	.string "Geschehen ist erfolgreich geladen.$$"

gSystemText_LoadingError:: @ 842C58F
	.string "Fehler beim Laden!\n"
	.string "Vorgang wird abgebrochen.$$$$$$$$"

gOtherText_Hoenn:: @ 842C5B5
	.string "HOENN$"

gText_OhABite:: @ 842C5BB
	.string "Oh! Da hat etwas angebissen!$"

gText_PokemonOnHook:: @ 842C5C7
	.string "Du hast ein POKéMON am Haken!{PAUSE_UNTIL_PRESS}$"

gText_NotEvenANibble:: @ 842C5E2
	.string "Nicht mal ein klitzekleines Knabbern...{PAUSE_UNTIL_PRESS}$"

gText_ItGotAway:: @ 842C5F9
	.string "Es ist entkommen...{PAUSE_UNTIL_PRESS}$$"

gText_XWillBeSentToY:: @ 842C60B
	.string "{STR_VAR_2} wird {STR_VAR_1} zugesandt.$"

gText_ByeByeVar1:: @ 842C622
	.string "Tschüss, {STR_VAR_2}!$"

gText_XSentOverY:: @ 842C62F
	.string "{STR_VAR_1} versandte {STR_VAR_3}.$"

gText_TakeGoodCareOfX:: @ 842C640
	.string "Kümmere dich gut um {STR_VAR_3}!$"

sText_Cancel::
	.string "ZUR.$"
sText_ChooseAPkmn::
	.string "Wähle ein POKéMON.$"
sText_Summary::
	.string "BERICHT$"
sText_Trade::
	.string "TAUSCH$"
sText_CancelTrade::
	.string "Tausch abbrechen?$"
TradeText_PressBToExit::
	.string "Mit B-Knopf beenden.$"
sText_Summary2::
	.string "BERICHT$"
sText_Trade2::
	.string "TAUSCH$"
sText_CommunicationStandby::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Verbindung...\n"
	.string "Bitte warten...$"
sText_TheTradeHasBeenCanceled::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Der Tausch\n"
	.string "wurde abgebrochen.$"
sText_OnlyPkmnForBattle::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Das ist das einzige\n"
	.string "POKéMON zum Kampf.$"
TradeText_NonTradablePoke::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Dein Freund kann\n"
	.string "das nicht senden.$"
sText_WaitingForYourFriend::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Dein Freund ist\n"
	.string "noch nicht fertig...$"
sText_YourFriendWantsToTrade::
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}Dein Freund möchte\n"
	.string "POKéMON tauschen.$"
	.string "$"

sText_IsThisTradeOkay:: @ 842C7B6
	.string "Ist der Tausch O.K.?$"

gOtherText_Terminator2:: @ 842C7CA
	.string "$"

gOtherText_ControlAndMiscText:: @ 842C7CB
	.string "\n$"
	.string "/$"
	.string "Lv.$"
	.string "---$"

gOtherText_FourQuestions:: @ 842C7D6
	.string "????$"

@ 842C7DB
	.include "data/text/easy_chat/group_name_strings.inc"

gOtherText_ThreeQuestions:: @ 842C890
	.string "???$"

gOtherText_From:: @ 842C894
	.string "Von $"

gText_MixingRecords:: @ 842C89A
	.string "Rekorddaten-Austausch...$"

gText_RecordMixingComplete:: @ 842C8AC
	.string "Rekorddaten-Austausch beendet.\n"
	.string "Vielen Dank für deine Geduld.$"

gOtherText_TrainersTrainerCard:: @ 842C8DC
	.string "TRAINERPASS von {PLAYER}$"

gText_HallOfFameDebut:: @ 842C8EC
	.string "ERSTE RUHMESHALLE:$"

gText_LinkCableBattles:: @ 842C8FF
	.string "KÄMPFE GG. FREUND/E  S.:            N.:$"

gText_BattleTower:: @ 842C924
	.string "DUELLTURM-S.:{CLEAR_TO 114}&{CLEAR_TO 152} IN FOLGE$"

gText_WonContestsWFriends:: @ 842C943
	.string "WETTB. MIT FREUND/EN S.:$"

gText_PokeblocksWithFriends:: @ 842C959
	.string "{POKEBLOCK} MIT FREUND/EN:$"

gText_PokemonTrades:: @ 842C96A
	.string "POKéMON-TAUSCH:$"

gOtherText_Boy:: @ 842C979
	.string "JUNGE$"

gOtherText_Girl:: @ 842C97D
	.string "MÄDEL$"
	.string "$"

gText_SavingPlayer:: @ 842C983
	.string "SPIELER$"

gText_SavingBadges:: @ 842C98A
	.string "ORDEN$"
	.string "$"

gText_SavingPokedex:: @ 842C992
	.string "POKéDEX$"

gText_SavingTime:: @ 842C99A
	.string "SPIELZEIT$"

gOtherText_SentToPC:: @ 842C9A4
	.string "{STR_VAR_1} wurde auf\n"
	.string "den PC übertragen.{PAUSE_UNTIL_PRESS}$"

OtherText_YourName::
	.string "DEIN NAME?$"
OtherText_BoxName::
	.string "NAME DER BOX?$"
OtherText_PokeName::
	.string "Kosename f. {STR_VAR_1}$"

gOtherText_Marco:: @ 842C9E1
	.string "MARCO$" @ polo

gOtherText_Coins:: @ 842C9E7
	.string "{STR_VAR_1} MÜNZ.$"

gOtherText_BattleResults:: @ 842C9F0
	.string "KAMPFERGEBNISSE von {PLAYER}$"

gOtherText_WinRecord:: @ 842CA04
	.string "GESAMT-REKORD S.:{STR_VAR_1} N.:{STR_VAR_2} P.:{STR_VAR_3}$"

gOtherText_WinLoseDraw:: @ 842CA20
	.string "SIEG      NIEDER.     PATT$"

gOtherText_SevenDashes:: @ 842CA3C
	.string "-------$"

gOtherText_FourDashes:: @ 842CA44
	.string "----$"

gOtherText_BattleTowerResults:: @ 842CA49
	.string "DUELLTURM-ERGEBNIS. von {PLAYER}$"

gOtherText_Lv50:: @ 842CA63
	.string "Lv. 50$"

gOtherText_Lv100:: @ 842CA69
	.string "Lv. 100$"

gOtherText_WinStreak:: @ 842CA70
	.string "{STR_VAR_1}S IN FOLGE$"

gOtherText_Current:: @ 842CA7B
	.string "MOMENTAN$"

gOtherText_Record:: @ 842CA83
	.string "REKORD$"

gOtherText_Prev:: @ 842CA8A
	.string "VORHER$"

gOtherText_QuitGamePrompt:: @ 842CA90
	.string "Spiel beenden?$"

gOtherText_MaxCoins:: @ 842CA9F
	.string "Du hast 9.999 MÜNZEN.$"

gOtherText_OutOfCoins:: @ 842CAB7
	.string "Du hast keine MÜNZEN mehr.\n"
	.string "Game over!$"

gOtherText_DontHaveThreeCoins:: @ 842CADB
	.string "Du hast keine drei MÜNZEN.$"

gOtherText_ReelTime:: @ 842CAF7
	.string "REEL TIME\n"
	.string "Hier hast du die Chance, in\n"
	.string "Blitzesschnelle zu gewinnen!\n"
	.string "Die Glücksrunde wiederholt\n"
	.string "sich, so oft die Zahl anzeigt.\n"
	.string "Sie endet mit einem BIG BONUS!$"

gDaycareText_GetAlongVeryWell::
	.string "Die beiden scheinen sich sehr zu mögen.$"
gDaycareText_GetAlong::
	.string "Die beiden scheinen sich zu vertragen.$"
gDaycareText_DontLikeOther::
	.string "Die beiden mögen sich nicht sehr.$"
gDaycareText_PlayOther::
	.string "Die beiden spielen lieber mit anderen\n"
	.string "POKéMON als miteinander.$"

gOtherText_NewLine2:: @ 842CC28
	.string "\n$"

gOtherText_CancelAndLv:: @ 842CC2A
	.string "KEINES$"
	.string "Lv.$"

gEggNickname:: @ 842CC34
	.string "EI$"

gOtherText_HatchedFromEgg:: @ 842CC38
	.string "{COLOR WHITE2}{HIGHLIGHT TRANSPARENT}{SHADOW DARK_GREY}{STR_VAR_1} schlüpfte aus dem EI!$"

gOtherText_NickHatchPrompt:: @ 842CC5A
	.string "{COLOR WHITE2}{HIGHLIGHT TRANSPARENT}{SHADOW DARK_GREY}Möchtest du dem geschlüpften\n"
	.string "{STR_VAR_1} einen Kosenamen geben?$"

OtherText_Battle::
	.string "KAMPF$"
OtherText_Contest::
	.string "WETTB.$"
OtherText_Type::
	.string "TYP/$"
OtherText_PP::
	.string "AP/$"
OtherText_Power::
	.string "STÄRKE$"
OtherText_Accuracy::
	.string "GENAU.$"
OtherText_Appeal::
	.string "AUSDRUCK$"
OtherText_Jam::
	.string "EINDRUCK$"

gOtherText_TeachWhichMove:: @ 842CCC7
	.string "{STR_VAR_1} welche Attacke beibr.?$"

gOtherText_TeachSpecificMove:: @ 842CCDF
	.string "{STR_VAR_2} beibringen?$"

gOtherText_PokeLearnedMove:: @ 842CCE9
	.string "{STR_VAR_1} erlernte {STR_VAR_2}.$"

gOtherText_DeleteOlderMove:: @ 842CCF8
	.string "{STR_VAR_1} versucht,\n"
	.string "{STR_VAR_2} zu erlernen.\p"
	.string "Aber {STR_VAR_1} kann nicht mehr\n"
	.string "als vier Attacken erlernen.\p"
	.string "Soll eine Attacke zu Gunsten von\n"
	.string "{STR_VAR_2} vergessen werden?$"

gOtherText_StopLearningMove:: @ 842CD65
	.string "{STR_VAR_2} nicht erlernen?$"

gOtherText_ForgotMove123:: @ 842CD77
	.string "{PAUSE 32}1, {PAUSE 15}2 {PAUSE 15}...{PAUSE 15}...{PAUSE 15}...{PAUSE 15} {PLAY_SE 0x38 0x00} Schwupp!\p$"

deuOtherText_ForgotAndLearned:: @ 843912f
	.string "{STR_VAR_1} vergaß {STR_VAR_3}.\p"
	.string "Und...\p"
	.string "{STR_VAR_1} erlernt {STR_VAR_2}.$"

gOtherText_ForgotOrDidNotLearnMove:: @ 842CDA3
	.string "{STR_VAR_1} hat {STR_VAR_2}\n"
	.string "nicht erlernt.$"

gOtherText_GiveUpTeachingMove:: @ 842CDDC
	.string "Soll {STR_VAR_1} keine neue Attacke\n"
	.string "erlernen?$"

gOtherText_WhichMoveToForget:: @ 842CE06
	.string "Welche Attacke soll vergessen\n"
	.string "werden?\p"
	.string "$"

gSystemText_ClearAllSaveDataPrompt:: @ 842CE27
	.string "Alle gesich. Datenbereiche löschen?$"

gSystemText_ClearingData:: @ 842CE42
	.string "Daten werden gelöscht...\n"
	.string "Bitte warten...$"

gText_AnnouncingResults:: @ 842CE60
	.string "Bekanntgabe der Ergebnisse!$"

gText_PreliminaryResults:: @ 842CE78
	.string "Ergebnisse 1. Durchlauf!$"

gText_Round2Results:: @ 842CE91
	.string "Ergebnisse 2. Durchlauf!$"

gText_ContestantsMonWon:: @ 842CEA2
	.string "Sieger ist: {STR_VAR_2} von {STR_VAR_1}!$"

gText_CommunicationStandby:: @ 842CEAF
	.string "Verbindung...$"

@ 842CEBF
	.include "data/text/landmarks.inc"

gOtherText_Terminator4:: @ 842D0E6
	.string ", $"

gOtherText_Nature:: @ 842D0E7
	.string "Wesen: $"

gOtherText_Comma:: @ 842D0F1
	.string ",$"

gOtherText_Met:: @ 842D0F3
	.string " (gef.)$"

gOtherText_Egg2:: @ 842D0FB
	.string " (EI)$"

gOtherText_ObtainedInTrade:: @ 842D103
	.string "im Tausch erhalten.$"

gOtherText_FatefulEncounter:: @ 842D118
	.string "schicksalhafte Begegnung.$"

gOtherText_Met2:: @ 842D12B
	.string " (gef.)$"

gOtherText_EggDayCare:: @ 842D133
	.string "Seltsames POKéMON-EI, das\n"
	.string "die PENSIONSLEITUNG fand.$"

gOtherText_EggNicePlace:: @ 842D164
	.string "Kurioses POKéMON-EI an\n"
	.string "hübschem Ort erhalten.$"

gOtherText_EggObtainedInTrade:: @ 842D197
	.string "Kurioses POKéMON-EI im\n"
	.string "Tausch erhalten.$"

gOtherText_EggHotSprings:: @ 842D1C3
	.string "Ein POKéMON-EI an den\n"
	.string "Heißen Quellen erhalten.$"

gOtherText_LinkStandby2:: @ 842D1EE
	.string "Verbindung...$"

gOtherText_YesNoAndPlayer:: @ 842D1FE
	.string "{COLOR DARK_GREY}{HIGHLIGHT WHITE2}{SHADOW LIGHT_GREY}JA\n"
	.string "NEIN$"

	.string "{HIGHLIGHT TRANSPARENT}SPIELER$"

	.string "{HIGHLIGHT TRANSPARENT}$"

gOtherText_HoennDex:: @ 842D21C
	.string "HOENN$"

gOtherText_NationalDex:: @ 842D222
	.string "NATIONALER$"

gOtherText_DiplomaCertificationGameFreak:: @ 842D22B
	.string "{HIGHLIGHT TRANSPARENT}Spieler: {CLEAR 16}{COLOR RED}{PLAYER}\n"
	.string "\n"
	.string "{COLOR DARK_GREY}{STR_VAR_1} POKéDEX\n"
	.string "Dies Dokument bescheinigt\n"
	.string "die erfolgreiche Vervoll-\n"
	.string "ständigung des POKéDEX.\n"
	.string "\n"
	.string "{CLEAR_TO 66}{COLOR RED}GAME FREAK$"

	.string "{COLOR RED}{HIGHLIGHT TRANSPARENT}GAME FREAK$"

gText_NumPlayerLink:: @ 842D2BC
	.string "{STR_VAR_1}SP-LINK$"

gOtherText_YesNoTerminating:: @ 842D2C5
	.string "JA\n"
	.string "NEIN$"

	.string "Abbruch$"

gOtherText_LinkNotFound:: @ 842D2D9
	.string "Kein(e) Link-Partner gefunden.\n"
	.string "Bitte erneut versuchen.\p$"

sText_BerryBlenderStart:: @ 842D307
	.string "BEERENMIXER wird gestartet.\p"
	.string "Wähle die BEERE aus dem BEUTEL,\n"
	.string "die in den BEERENMIXER soll.\p$"

sText_WasMade:: @ 842D368
	.string "{STR_VAR_1} wurde gemixt!$"

sText_PressAToStart:: @ 842D373
	.string "Drücke A-Knopf zum Starten.$"

sText_PleaseWaitAWhile:: @ 842D390
	.string "Bitte hab noch ein wenig Geduld.$"

gOtherText_LinkStandby3:: @ 842D3A5
	.string "Verbindung...$"

sText_WouldLikeToBlendAnotherBerry:: @ 842D3B5
	.string "Möchtest du noch eine BEERE mixen?$"

sText_RunOutOfBerriesForBlending:: @ 842D3DC
	.string "Du hast keine BEEREN mehr zum\n"
	.string "Mixen im BEERENMIXER.\p"
	.string "$"

sText_YourPokeblockCaseIsFull:: @ 842D41A
	.string "Deine {POKEBLOCK}-BOX ist voll.\p$"

sText_HasNoBerriesToPut:: @ 842D434
	.string "{PLAYER} hat keine BEEREN\n"
	.string "für den BEERENMIXER.$"

sText_ApostropheSPokeblockCaseIsFull:: @ 842D461
	.string "{POKEBLOCK}-BOX von {PLAYER} \n"
	.string "ist voll.\n$"

sText_BlendingResults:: @ 842D479
	.string "MIX-RESULTATE$"

	.string "VERW. BEERE$"

sText_SpaceBerry:: @ 842D498
	.string "{STR_VAR_1}BEERE$"

sText_Time:: @ 842D49F
	.string "ZEIT BENÖTIGT$"

sText_Min:: @ 842D4AD
	.string "MIN.$"

sText_Sec:: @ 842D4B2
	.string "SEK.$"

sText_MaximumSpeed:: @ 842D4B7
	.string "MAXIMALTEMPO$"

sText_RPM:: @ 842D4C5
	.string " RPM$"

	.string ",$"

sText_Ranking:: @ 842D4CB
	.string "STUFE$"

sText_TheLevelIs:: @ 842D4D3
	.string "Der Level ist $"

sText_TheFeelIs:: @ 842D4E1
	.string " und die Würze ist $"

sText_Dot2:: @ 842D4F4
	.string ".$"

gText_Day:: @ 842D4F6
	.string "TAG$"
	.string ":$"

gOtherText_OK:: @ 842D4FC
	.string "O.K.$"

gOtherText_CorrectTimePrompt:: @ 842D4FF
	.string "Ist die Uhrzeit korrekt?$"
