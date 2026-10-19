#include "constants/moves.h"

static const u8 sText_Trainer1LoseText[] = _("{STRING 33}");
static const u8 sText_PkmnGainedEXP[] = _("{STRING 0} erhält{PLAYER}\n{STRING 42} E-Punkte!\p");
static const u8 sText_EmptyString4[] = _("");
static const u8 sText_ABoosted[] = _(" spezielle");
static const u8 sText_PkmnGrewToLv[] = _("{STRING 0} erreicht\nLv. {PLAYER}!{UNKNOWN_A}\p");
static const u8 sText_PkmnLearnedMove[] = _("{STRING 0} erlernt\n{PLAYER}!{UNKNOWN_A}\p");
static const u8 sText_TryToLearnMove1[] = _("{STRING 0} versucht,\n{PLAYER} zu lernen.\p");
static const u8 sText_TryToLearnMove2[] = _("Aber {STRING 0} kann nicht\nmehr als vier Attacken erlernen.\p");
static const u8 sText_TryToLearnMove3[] = _("Soll eine Attacke zu Gunsten von\n{PLAYER} vergessen werden?");
static const u8 sText_PkmnForgotMove[] = _("{STRING 0} hat\n{PLAYER} vergessen!\p");
static const u8 sText_StopLearningMove[] = _("{PAUSE 32}{PLAYER}\nnicht erlernen?");
static const u8 sText_DidNotLearnMove[] = _("{STRING 0} hat {PLAYER}\nnicht erlernt.\p");
static const u8 sText_UseNextPkmn[] = _("Nächstes POKéMON einsetzen?");
static const u8 sText_AttackMissed[] = _("Attacke von {EVIL_LEGENDARY}\nging daneben!");
static const u8 sText_PkmnProtectedItself[] = _("{GOOD_LEGENDARY}\nschützt sich selbst!");
static const u8 sText_AvoidedDamage[] = _("{GOOD_LEGENDARY} vermeidet\nSchaden mit {STRING 22}!");
static const u8 sText_PkmnMakesGroundMiss[] = _("{GOOD_LEGENDARY} wehrt BODEN-\nAttacken mit {STRING 22} ab!");
static const u8 sText_PkmnAvoidedAttack[] = _("{GOOD_LEGENDARY} wehrt die\nAttacke ab!");
static const u8 sText_ItDoesntAffect[] = _("Es hat keine Wirkung auf\n{GOOD_LEGENDARY}...");
static const u8 sText_AttackerFainted[] = _("{EVIL_LEGENDARY}\nwurde besiegt!\p");
static const u8 sText_TargetFainted[] = _("{GOOD_LEGENDARY}\nwurde besiegt!\p");
static const u8 sText_PlayerGotMoney[] = _("{STRING 32} gewinnt ¥{STRING 0}!\p");
static const u8 sText_PlayerWhiteout[] = _("{STRING 32} hat kein\nkampffähiges POKéMON mehr!\p");
static const u8 sText_PlayerWhiteout2[] = _("{STRING 32} fällt in Ohnmacht!{PAUSE_UNTIL_PRESS}");
static const u8 sText_PreventsEscape[] = _("{STRING 16} verhindert\neine Flucht mit {STRING 23}!\p");
static const u8 sText_CantEscape2[] = _("Flucht unmöglich!\p");
static const u8 sText_AttackerCantEscape[] = _("{EVIL_LEGENDARY}\nkann nicht fliehen!");
static const u8 sText_HitXTimes[] = _("{STRING 0}-mal getroffen!");
static const u8 sText_PkmnFellAsleep[] = _("{STRING 14}\nist eingeschlafen!");
static const u8 sText_PkmnMadeSleep[] = _("{STRING 23} von\n{STRING 16} lässt\l{STRING 14} einschlafen!");
static const u8 sText_PkmnAlreadyAsleep[] = _("{GOOD_LEGENDARY} schläft\nbereits!");
static const u8 sText_PkmnAlreadyAsleep2[] = _("{EVIL_LEGENDARY} schläft\nbereits!");
static const u8 sText_PkmnWasntAffected[] = _("{GOOD_LEGENDARY}\nist unversehrt!");
static const u8 sText_PkmnWasPoisoned[] = _("{STRING 14}\nwurde vergiftet!");
static const u8 sText_PkmnPoisonedBy[] = _("{STRING 23} von\n{STRING 16} vergiftete\l{STRING 14}!");
static const u8 sText_PkmnHurtByPoison[] = _("{EVIL_LEGENDARY} wurde durch\nGift verletzt!");
static const u8 sText_PkmnAlreadyPoisoned[] = _("{GOOD_LEGENDARY} ist bereits\nvergiftet.");
static const u8 sText_PkmnBadlyPoisoned[] = _("{STRING 14} wurde schwer\nvergiftet!");
static const u8 sText_PkmnEnergyDrained[] = _("{GOOD_LEGENDARY} wurde\nEnergie abgesaugt!");
static const u8 sText_PkmnWasBurned[] = _("{STRING 14} brennt!");
static const u8 sText_PkmnBurnedBy[] = _("{STRING 23} von\n{STRING 16} verbrennt\l{STRING 14}!");
static const u8 sText_PkmnHurtByBurn[] = _("Die Verbrennung schadet\n{EVIL_LEGENDARY}!");
static const u8 sText_PkmnAlreadyHasBurn[] = _("{GOOD_LEGENDARY} brennt\nbereits.");
static const u8 sText_PkmnWasFrozen[] = _("{STRING 14} erstarrt\nzu Eis!");
static const u8 sText_PkmnFrozenBy[] = _("{STRING 14} wurde durch\n{STRING 23} von\l{STRING 16} eingefroren!");
static const u8 sText_PkmnIsFrozen[] = _("{EVIL_LEGENDARY} ist\neingefroren!");
static const u8 sText_PkmnWasDefrosted[] = _("{GOOD_LEGENDARY} wurde\naufgetaut!");
static const u8 sText_PkmnWasDefrosted2[] = _("{EVIL_LEGENDARY} wurde\naufgetaut!");
static const u8 sText_PkmnWasDefrostedBy[] = _("{EVIL_LEGENDARY} wurde\ndurch {STRING 17} aufgetaut!");
static const u8 sText_PkmnWasParalyzed[] = _("{STRING 14} ist\nparalysiert! Es greift\leventuell nicht an!");
static const u8 sText_PkmnWasParalyzedBy[] = _("{STRING 23} von\n{STRING 16} paralysierte\p{STRING 14}!\nEs greift eventuell nicht an!");
static const u8 sText_PkmnIsParalyzed[] = _("{EVIL_LEGENDARY} ist\nparalysiert! Es kann\lnicht angreifen!");
static const u8 sText_PkmnIsAlreadyParalyzed[] = _("{GOOD_LEGENDARY} ist\nbereits paralysiert!");
static const u8 sText_PkmnHealedParalysis[] = _("{GOOD_LEGENDARY} wurde von der\nParalyse geheilt!");
static const u8 sText_PkmnDreamEaten[] = _("Der Traum von {GOOD_LEGENDARY}\nwurde gefressen!");
static const u8 sText_StatsWontIncrease[] = _("{STRING 0} von\n{EVIL_LEGENDARY} kann nicht\lmehr erhöht werden!");
static const u8 sText_StatsWontDecrease[] = _("{STRING 0} von\n{GOOD_LEGENDARY} kann nicht\lweiter gesenkt werden!");
static const u8 sText_TeamStoppedWorking[] = _("{STRING 0} in deinem Team\nfunktioniert nicht mehr!");
static const u8 sText_FoeStoppedWorking[] = _("{STRING 0} des Gegners\nfunktioniert nicht mehr!");
static const u8 sText_PkmnIsConfused[] = _("{EVIL_LEGENDARY} ist\nverwirrt!");
static const u8 sText_PkmnHealedConfusion[] = _("{EVIL_LEGENDARY} ist nicht\nmehr verwirrt!");
static const u8 sText_PkmnWasConfused[] = _("{STRING 14} wurde\nverwirrt!");
static const u8 sText_PkmnAlreadyConfused[] = _("{GOOD_LEGENDARY} ist\nbereits verwirrt!");
static const u8 sText_PkmnFellInLove[] = _("{GOOD_LEGENDARY}\nhat sich verliebt!");
static const u8 sText_PkmnInLove[] = _("{EVIL_LEGENDARY} hat sich in\n{STRING 16} verliebt!");
static const u8 sText_PkmnImmobilizedByLove[] = _("{EVIL_LEGENDARY} ist starr\nvor Liebe!");
static const u8 sText_PkmnBlownAway[] = _("{GOOD_LEGENDARY} wurde\nweggeweht!");
static const u8 sText_PkmnChangedType[] = _("{EVIL_LEGENDARY} verwandelt\nsich zu Typ {STRING 0}!");
static const u8 sText_PkmnFlinched[] = _("{EVIL_LEGENDARY} schreckt\nzurück!");
static const u8 sText_PkmnRegainedHealth[] = _("{GOOD_LEGENDARY} erholt sich!");
static const u8 sText_PkmnHPFull[] = _("{GOOD_LEGENDARY} hat alle KP!");
static const u8 sText_PkmnRaisedSpDef[] = _("{STRING 17} von\n{STRING 38} erhöht\ldie SP. VER.");
static const u8 sText_PkmnRaisedSpDefALittle[] = _("Die SP. VER. wird etwas durch\n{STRING 17} von\l{STRING 38} erhöht.");
static const u8 sText_PkmnRaisedDef[] = _("{STRING 17} von\n{STRING 38}\lerhöht die VERTEIDIGUNG!");
static const u8 sText_PkmnRaisedDefALittle[] = _("{STRING 17} von\n{STRING 38}\lerhöht etwas die VERTEIDIGUNG!");
static const u8 sText_PkmnCoveredByVeil[] = _("{STRING 38} wird von\neinem Schleier umhüllt!");
static const u8 sText_PkmnUsedSafeguard[] = _("{GOOD_LEGENDARY} wird durch\nBODYGUARD geschützt!");
static const u8 sText_PkmnSafeguardExpired[] = _("BODYGUARD von\n{STRING 40} lässt nach!");
static const u8 sText_PkmnWentToSleep[] = _("{EVIL_LEGENDARY} ist\neingeschlafen!");
static const u8 sText_PkmnSleptHealthy[] = _("{EVIL_LEGENDARY} schläft und\nerholt sich!");
static const u8 sText_PkmnWhippedWhirlwind[] = _("{EVIL_LEGENDARY} entfacht\neinen Wirbelwind!");
static const u8 sText_PkmnTookSunlight[] = _("{EVIL_LEGENDARY} absorbiert\nSonnenlicht!");
static const u8 sText_PkmnLoweredHead[] = _("{EVIL_LEGENDARY} zieht seinen\nKopf ein!");
static const u8 sText_PkmnIsGlowing[] = _("{EVIL_LEGENDARY} leuchtet!");
static const u8 sText_PkmnFlewHigh[] = _("{EVIL_LEGENDARY} fliegt\nhoch empor!");
static const u8 sText_PkmnDugHole[] = _("{EVIL_LEGENDARY} gräbt sich\nein!");
static const u8 sText_PkmnHidUnderwater[] = _("{EVIL_LEGENDARY} taucht\nunter!");
static const u8 sText_PkmnSprangUp[] = _("{EVIL_LEGENDARY} springt auf!");
static const u8 sText_PkmnSqueezedByBind[] = _("{EVIL_LEGENDARY} setzt bei\n{GOOD_LEGENDARY}\lKLAMMERGRIFF ein!");
static const u8 sText_PkmnTrappedInVortex[] = _("{GOOD_LEGENDARY} wurde in\neinem Strudel gefangen!");
static const u8 sText_PkmnTrappedBySandTomb[] = _("{GOOD_LEGENDARY} wurde durch\nSANDGRAB gefangen!");
static const u8 sText_PkmnWrappedBy[] = _("{GOOD_LEGENDARY} wurde von\n{EVIL_LEGENDARY} umWICKELt!");
static const u8 sText_PkmnClamped[] = _("{GOOD_LEGENDARY} wurde von\n{EVIL_LEGENDARY} geSCHNAPPT!");
static const u8 sText_PkmnHurtBy[] = _("{EVIL_LEGENDARY} wurde durch\n{STRING 0} verletzt!");
static const u8 sText_PkmnFreedFrom[] = _("{EVIL_LEGENDARY} wurde von\n{STRING 0} befreit!");
static const u8 sText_PkmnCrashed[] = _("{EVIL_LEGENDARY} macht weiter\nund bricht zusammen!");
const u8 gText_PkmnShroudedInMist[] = _("{STRING 38} wird in\nWEISSNEBEL gehüllt!");
static const u8 sText_PkmnProtectedByMist[] = _("{STRING 16} wird durch\nWEISSNEBEL geschützt!");
const u8 gText_PkmnGettingPumped[] = _("{EVIL_LEGENDARY} pumpt\nsich auf!");
static const u8 sText_PkmnHitWithRecoil[] = _("{EVIL_LEGENDARY} wurde vom\nRückstoß getroffen!");
static const u8 sText_PkmnProtectedItself2[] = _("{EVIL_LEGENDARY} schützt\nsich selbst!");
static const u8 sText_PkmnBuffetedBySandstorm[] = _("{EVIL_LEGENDARY} wird vom\nSandsturm getroffen!");
static const u8 sText_PkmnPeltedByHail[] = _("{EVIL_LEGENDARY} wird vom\nHAGELSTURM getroffen!");
static const u8 sText_PkmnsXWoreOff[] = _("{STRING 0} von\n{STRING 36} lässt nach!");
static const u8 sText_PkmnSeeded[] = _("{GOOD_LEGENDARY} wurde\nbepflanzt!");
static const u8 sText_PkmnEvadedAttack[] = _("{GOOD_LEGENDARY} ist\nausgewichen!");
static const u8 sText_PkmnSappedByLeechSeed[] = _("EGELSAMEN schadet\n{EVIL_LEGENDARY}!");
static const u8 sText_PkmnFastAsleep[] = _("{EVIL_LEGENDARY} schläft tief\nund fest.");
static const u8 sText_PkmnWokeUp[] = _("{EVIL_LEGENDARY} ist\naufgewacht!");
static const u8 sText_PkmnUproarKeptAwake[] = _("AUFRUHR von {STRING 16}\nhält es wach!");
static const u8 sText_PkmnWokeUpInUproar[] = _("{EVIL_LEGENDARY} wird durch\nAUFRUHR wach!");
static const u8 sText_PkmnCausedUproar[] = _("{EVIL_LEGENDARY} verursacht\nAUFRUHR!");
static const u8 sText_PkmnMakingUproar[] = _("{EVIL_LEGENDARY} macht\neinen AUFRUHR!");
static const u8 sText_PkmnCalmedDown[] = _("{EVIL_LEGENDARY}\nberuhigt sich.");
static const u8 sText_PkmnCantSleepInUproar[] = _("{GOOD_LEGENDARY} kann bei dem\nAUFRUHR nicht schlafen!");
static const u8 sText_PkmnStockpiled[] = _("{EVIL_LEGENDARY} HORTET\n{STRING 0}!");
static const u8 sText_PkmnCantStockpile[] = _("{EVIL_LEGENDARY} kann nicht\nweiter HORTEN!");
static const u8 sText_PkmnCantSleepInUproar2[] = _("{GOOD_LEGENDARY} kann bei dem\nAUFRUHR nicht schlafen!");
static const u8 sText_UproarKeptPkmnAwake[] = _("Aber der AUFRUHR hält\n{GOOD_LEGENDARY} wach!");
static const u8 sText_PkmnStayedAwakeUsing[] = _("{GOOD_LEGENDARY} hält sich mit\n{STRING 22} wach!");
static const u8 sText_PkmnStoringEnergy[] = _("{EVIL_LEGENDARY} speichert\nEnergie!");
static const u8 sText_PkmnUnleashedEnergy[] = _("{EVIL_LEGENDARY} erzeugt\nEnergie!");
static const u8 sText_PkmnFatigueConfusion[] = _("{EVIL_LEGENDARY} ist vor\nErschöpfung verwirrt!");
static const u8 sText_PlayerPickedUpMoney[] = _("{STRING 32} hebt\n¥{STRING 0} auf!\p");
static const u8 sText_PkmnUnaffected[] = _("{GOOD_LEGENDARY} ist\nunversehrt!");
static const u8 sText_PkmnTransformedInto[] = _("{EVIL_LEGENDARY} verwandelt\nsich in {STRING 0}!");
static const u8 sText_PkmnMadeSubstitute[] = _("{EVIL_LEGENDARY} setzt einen\nDELEGATOR ein!");
static const u8 sText_PkmnHasSubstitute[] = _("{EVIL_LEGENDARY} hat bereits\neinen DELEGATOR!");
static const u8 sText_SubstituteDamaged[] = _("Der DELEGATOR steckt den\nSchlag für {GOOD_LEGENDARY}\lein!\p");
static const u8 sText_PkmnSubstituteFaded[] = _("DELEGATOR von\n{GOOD_LEGENDARY} lässt nach!\p");
static const u8 sText_PkmnMustRecharge[] = _("{EVIL_LEGENDARY} muss sich\nwieder aufladen!");
static const u8 sText_PkmnRageBuilding[] = _("{GOOD_LEGENDARY} verfällt in\nRASEREI!");
static const u8 sText_PkmnMoveWasDisabled[] = _("{STRING 0} von\n{GOOD_LEGENDARY}\lwurde blockiert!");
static const u8 sText_PkmnMoveDisabledNoMore[] = _("{EVIL_LEGENDARY} ist nicht\nmehr blockiert!");
static const u8 sText_PkmnGotEncore[] = _("{GOOD_LEGENDARY} gibt\neine ZUGABE!");
static const u8 sText_PkmnEncoreEnded[] = _("ZUGABE von {EVIL_LEGENDARY}\nist beendet!");
static const u8 sText_PkmnTookAim[] = _("{EVIL_LEGENDARY} zielt\nauf {GOOD_LEGENDARY}!");
static const u8 sText_PkmnSketchedMove[] = _("{EVIL_LEGENDARY} setzt\nNACHAHMER bei {STRING 0} ein!");
static const u8 sText_PkmnTryingToTakeFoe[] = _("{EVIL_LEGENDARY} versucht den\nGegner mit sich zu nehmen!");
static const u8 sText_PkmnTookFoe[] = _("{GOOD_LEGENDARY} nimmt\n{EVIL_LEGENDARY} mit sich!");
static const u8 sText_PkmnReducedPP[] = _("{STRING 0} von\n{GOOD_LEGENDARY} wird um\l{PLAYER} reduziert!");
static const u8 sText_PkmnStoleItem[] = _("{EVIL_LEGENDARY} stiehlt\n{STRING 19} von\l{GOOD_LEGENDARY}!");
static const u8 sText_TargetCantEscapeNow[] = _("{GOOD_LEGENDARY} kann\nnicht fliehen!");
static const u8 sText_PkmnFellIntoNightmare[] = _("{GOOD_LEGENDARY} bekommt\nNACHTMAHR!");
static const u8 sText_PkmnLockedInNightmare[] = _("{EVIL_LEGENDARY} ist in\nNACHTMAHR gefangen!");
static const u8 sText_PkmnLaidCurse[] = _("{EVIL_LEGENDARY} nimmt einen\nTeil seiner KP und legt einen\lFLUCH auf {GOOD_LEGENDARY}!");
static const u8 sText_PkmnAfflictedByCurse[] = _("{EVIL_LEGENDARY} wurde durch\nFLUCH verletzt!");
static const u8 sText_SpikesScattered[] = _("POKéMON-Team von {STRING 37}\nverteilt STACHELN um sich!");
static const u8 sText_PkmnHurtBySpikes[] = _("{STRING 16} wurde durch\nSTACHLER verletzt!");
static const u8 sText_PkmnIdentified[] = _("{EVIL_LEGENDARY} erkennt\n{GOOD_LEGENDARY}!");
static const u8 sText_PkmnPerishCountFell[] = _("ABGESANG von {EVIL_LEGENDARY}\nsteht bei {STRING 0}!");
static const u8 sText_PkmnBracedItself[] = _("{EVIL_LEGENDARY} macht sich\nbereit!");
static const u8 sText_PkmnEnduredHit[] = _("{GOOD_LEGENDARY} setzt\nAUSDAUER ein!");
static const u8 sText_MagnitudeStrength[] = _("INTENSITÄT {STRING 0}!");
static const u8 sText_PkmnCutHPMaxedAttack[] = _("{EVIL_LEGENDARY} nutzt seine\nKP und hebt den ANGR-Wert!");
static const u8 sText_PkmnCopiedStatChanges[] = _("{EVIL_LEGENDARY} kopiert die\nStatusveränderungen\lvon {GOOD_LEGENDARY}!");
static const u8 sText_PkmnGotFree[] = _("{EVIL_LEGENDARY} befreit\nsich von {STRING 0}\lvon {GOOD_LEGENDARY}!");
static const u8 sText_PkmnShedLeechSeed[] = _("{EVIL_LEGENDARY} befreit sich\nvon EGELSAMEN!");
static const u8 sText_PkmnBlewAwaySpikes[] = _("{EVIL_LEGENDARY} blies den\nSTACHLER weg!");
static const u8 sText_PkmnFledFromBattle[] = _("{EVIL_LEGENDARY} ist\ngeflüchtet!");
static const u8 sText_PkmnForesawAttack[] = _("{EVIL_LEGENDARY} sah eine\nAttacke voraus!");
static const u8 sText_PkmnTookAttack[] = _("{GOOD_LEGENDARY} wurde von\nder Attacke {STRING 0} getroffen!");
static const u8 sText_PkmnChoseXAsDestiny[] = _("{EVIL_LEGENDARY} wählte\n{STRING 17} als Vorhersehung aus!");
static const u8 sText_PkmnAttack[] = _("Angriff von {STRING 0}!");
static const u8 sText_PkmnCenterAttention[] = _("{EVIL_LEGENDARY} zieht alle\nAufmerksamkeit auf sich!");
static const u8 sText_PkmnChargingPower[] = _("{EVIL_LEGENDARY} lädt\nsich auf!");
static const u8 sText_NaturePowerTurnedInto[] = _("NATUR-KRAFT wurde zu\n{STRING 17}!");
static const u8 sText_PkmnStatusNormal[] = _("Status von {EVIL_LEGENDARY}\nwird normal!");
static const u8 sText_PkmnSubjectedToTorment[] = _("{GOOD_LEGENDARY} wird von\nFOLTERKNECHT unterworfen!");
static const u8 sText_PkmnTighteningFocus[] = _("{EVIL_LEGENDARY} verstärkt\nseinen Fokus!");
static const u8 sText_PkmnFellForTaunt[] = _("{GOOD_LEGENDARY} fällt auf\nVERHÖHNER herein!");
static const u8 sText_PkmnReadyToHelp[] = _("{EVIL_LEGENDARY} will\n{GOOD_LEGENDARY} helfen!");
static const u8 sText_PkmnSwitchedItems[] = _("{EVIL_LEGENDARY} tauscht\nItems mit anderem PKMN!");
static const u8 sText_PkmnObtainedX[] = _("{EVIL_LEGENDARY} erhält\n{STRING 0}.");
static const u8 sText_PkmnObtainedX2[] = _("{GOOD_LEGENDARY} erhält\n{PLAYER}.");
static const u8 sText_PkmnObtainedXYObtainedZ[] = _("{EVIL_LEGENDARY} erhält\n{STRING 0}.\p{GOOD_LEGENDARY} erhält\n{PLAYER}.");
static const u8 sText_PkmnCopiedFoe[] = _("{EVIL_LEGENDARY}\nkopiert {STRING 22} von\l{GOOD_LEGENDARY}!");
static const u8 sText_PkmnMadeWish[] = _("{EVIL_LEGENDARY} spricht einen\nWUNSCHTRAUM aus!");
static const u8 sText_PkmnWishCameTrue[] = _("WUNSCHTRAUM von\n{STRING 0} erfüllt sich!");
static const u8 sText_PkmnPlantedRoots[] = _("{EVIL_LEGENDARY} pflanzt seine\nWurzeln!");
static const u8 sText_PkmnAbsorbedNutrients[] = _("{EVIL_LEGENDARY} nimmt über\nseine Wurzeln Nährstoffe auf!");
static const u8 sText_PkmnAnchoredItself[] = _("{GOOD_LEGENDARY}\nverankert sich mit seinen Wurzeln!");
static const u8 sText_PkmnWasMadeDrowsy[] = _("{EVIL_LEGENDARY} macht\n{GOOD_LEGENDARY} schläfrig!");
static const u8 sText_PkmnKnockedOff[] = _("{EVIL_LEGENDARY}\nentreißt {STRING 19} von\l{GOOD_LEGENDARY}!");
static const u8 sText_PkmnSwappedAbilities[] = _("{EVIL_LEGENDARY} tauscht die\nFähigkeiten mit einem PKMN!");
static const u8 sText_PkmnSealedOpponentMove[] = _("{EVIL_LEGENDARY} versiegelt\ndie Attacke/n des Gegners!");
static const u8 sText_PkmnWantsGrudge[] = _("{EVIL_LEGENDARY} möchte, dass\nder Gegner ein NACHSPIEL erträgt!");
static const u8 sText_PkmnLostPPGrudge[] = _("{STRING 0} von\n{EVIL_LEGENDARY} hat aufgrund\lvon NACHSPIEL alle AP verloren!");
static const u8 sText_PkmnShroudedItself[] = _("{EVIL_LEGENDARY} verhüllt sich\nselbst in {STRING 17}!");
static const u8 sText_PkmnMoveBounced[] = _("{STRING 17} von\n{EVIL_LEGENDARY} prallte am\lMAGIEMANTEL ab!");
static const u8 sText_PkmnWaitsForTarget[] = _("{EVIL_LEGENDARY} wartet auf\neine gegnerische Attacke!");
static const u8 sText_PkmnSnatchedMove[] = _("{GOOD_LEGENDARY} ÜBERNAHM\nAttacke von {STRING 16}!");
static const u8 sText_ElectricityWeakened[] = _("Die Stärke der Elektrizität\nwurde geschwächt!");
static const u8 sText_FireWeakened[] = _("Die Stärke des Feuers\nwurde geschwächt!");
static const u8 sText_XFoundOneY[] = _("{EVIL_LEGENDARY} hat 1\n{STRING 19} gefunden!");
static const u8 sText_SoothingAroma[] = _("Ein wohltuendes Aroma\nbreitet sich aus!");
static const u8 sText_ItemsCantBeUsedNow[] = _("Hier können Items nicht eingesetzt\nwerden.{PAUSE 64}");
static const u8 sText_ForXCommaYZ[] = _("Für {STRING 16} war\n{STRING 19} {STRING 0}");
static const u8 sText_PkmnUsedXToGetPumped[] = _("{STRING 16} setzt\n{STRING 19} ein, um zu drängeln!");
static const u8 sText_PkmnLostFocus[] = _("{EVIL_LEGENDARY} hat seinen\nFokus verloren und kann\lnicht angreifen!");
static const u8 sText_PkmnWasDraggedOut[] = _("{GOOD_LEGENDARY} wurde\nauserwählt!\p");
static const u8 sText_TheWallShattered[] = _("Die Mauer brach zusammen!");
static const u8 sText_ButNoEffect[] = _("Es ist wirkungslos!");
static const u8 sText_PkmnHasNoMovesLeft[] = _("{STRING 15} hat keine\nAttacken mehr übrig!\p");
static const u8 sText_PkmnMoveIsDisabled[] = _("{STRING 17} von\n{STRING 15} ist blockiert!\p");
static const u8 sText_PkmnCantUseMoveTorment[] = _("{STRING 15} kann\naufgrund von FOLTERKNECHT\pdie Attacke nicht 2-mal\nhintereinander einsetzen!");
static const u8 sText_PkmnCantUseMoveTaunt[] = _("{STRING 15} kann\n{STRING 17} nach VERHÖHNER\lnicht einsetzen!\p");
static const u8 sText_PkmnCantUseMoveSealed[] = _("{STRING 15} kann die ver-\nsiegelte Attacke {STRING 17}\lnicht einsetzen!\p");
static const u8 sText_PkmnMadeItRain[] = _("{STRING 23} von\n{STRING 16} lässt\les regnen!");
static const u8 sText_PkmnRaisedSpeed[] = _("{STRING 23} von\n{STRING 16} erhöht\lseine INIT.!");
static const u8 sText_PkmnProtectedBy[] = _("{GOOD_LEGENDARY} wurde durch\n{STRING 22} geschützt!");
static const u8 sText_PkmnPreventsUsage[] = _("{STRING 22} von\n{GOOD_LEGENDARY} hindert\p{EVIL_LEGENDARY} daran,\n{STRING 17} einzusetzen!");
static const u8 sText_PkmnRestoredHPUsing[] = _("{GOOD_LEGENDARY} füllt KP mit\nHilfe von {STRING 22} auf!");
static const u8 sText_PkmnsXMadeYUseless[] = _("{STRING 22} von\n{GOOD_LEGENDARY} hebt die\lWirkung von {STRING 17} auf!");
static const u8 sText_PkmnChangedTypeWith[] = _("{STRING 22} von\n{GOOD_LEGENDARY} macht es zu\leinem {STRING 0}-Typ!");
static const u8 sText_PkmnPreventsParalysisWith[] = _("{STRING 22} von\n{STRING 14} verhindert\leine Paralyse!");
static const u8 sText_PkmnPreventsRomanceWith[] = _("{STRING 22} von\n{GOOD_LEGENDARY} verhindert\leine Romanze!");
static const u8 sText_PkmnPreventsPoisoningWith[] = _("{STRING 22} von\n{STRING 14} verhindert\leine Vergiftung!");
static const u8 sText_PkmnPreventsConfusionWith[] = _("{STRING 22} von\n{GOOD_LEGENDARY} verhindert\lVerwirrung!");
static const u8 sText_PkmnRaisedFirePowerWith[] = _("{STRING 22} von\n{GOOD_LEGENDARY} erhöht die\lFEUER-Kraft!");
static const u8 sText_PkmnAnchorsItselfWith[] = _("{GOOD_LEGENDARY} verankert\nsich mit Hilfe von {STRING 22}!");
static const u8 sText_PkmnCutsAttackWith[] = _("{STRING 23} von\n{STRING 16} vermindert\lANGRIFF von {GOOD_LEGENDARY}!");
static const u8 sText_PkmnPreventsStatLossWith[] = _("{STRING 23} von\n{STRING 16} verhindert\lStatusveränderungen!");
static const u8 sText_PkmnHurtsWith[] = _("{STRING 22} von\n{GOOD_LEGENDARY} verletzt\l{EVIL_LEGENDARY}!");
static const u8 sText_PkmnTraced[] = _("FÄHRTE von\n{STRING 16}\perkennt {PLAYER}\nvon {STRING 0}!");
static const u8 sText_PkmnsXPreventsBurns[] = _("{STRING 24} von\n{STRING 14} verhindert\lVerbrennung!");
static const u8 sText_PkmnsXBlocksY[] = _("{STRING 22} von\n{GOOD_LEGENDARY} blockiert\l{STRING 17}!");
static const u8 sText_PkmnsXBlocksY2[] = _("{STRING 23} von\n{STRING 16} blockiert\l{STRING 17}!");
static const u8 sText_PkmnsXRestoredHPALittle2[] = _("{STRING 21} von\n{EVIL_LEGENDARY} füllt\leinige KP auf!");
static const u8 sText_PkmnsXWhippedUpSandstorm[] = _("{STRING 23} von\n{STRING 16} entfacht\leinen Sandsturm!");
static const u8 sText_PkmnsXIntensifiedSun[] = _("{STRING 23} von\n{STRING 16} intensiviert\ldie Sonnenstrahlen!");
static const u8 sText_PkmnsXPreventsYLoss[] = _("{STRING 23} von\n{STRING 16} verhindert \lden Verlust von {STRING 0}!");
static const u8 sText_PkmnsXInfatuatedY[] = _("{EVIL_LEGENDARY} ist vernarrt\nin {STRING 22} von\l{GOOD_LEGENDARY}!");
static const u8 sText_PkmnsXMadeYIneffective[] = _("{STRING 22} von\n{GOOD_LEGENDARY} macht\l{STRING 17} wirkungslos!");
static const u8 sText_PkmnsXCuredYProblem[] = _("{STRING 23} von\n{STRING 16} heilte sein\lProblem mit {STRING 0}!");
static const u8 sText_ItSuckedLiquidOoze[] = _("Es saugte\nKLOAKENSOSSE auf!");
static const u8 sText_PkmnTransformed[] = _("{STRING 16}\nverwandelte sich!");
static const u8 sText_PkmnsXTookAttack[] = _("{STRING 22} von\n{GOOD_LEGENDARY} übernahm\lden Angriff!");
const u8 gText_PkmnsXPreventsSwitching[] = _("{STRING 20} von\n{STRING 0} verhindert\pWechsel!\p");
static const u8 sText_PreventedFromWorking[] = _("{STRING 22} von\n{GOOD_LEGENDARY} macht\p{STRING 0} von\n{STRING 16} wirkungslos!");
static const u8 sText_PkmnsXMadeItIneffective[] = _("{STRING 23} von\n{STRING 16} machte\les wirkungslos!");
static const u8 sText_PkmnsXPreventsFlinching[] = _("{STRING 24} von\n{STRING 14} verhindert\lein Zurückschrecken!");
static const u8 sText_PkmnsXPreventsYsZ[] = _("{STRING 21} von\n{EVIL_LEGENDARY} macht\p{STRING 22} von\n{GOOD_LEGENDARY} wirkungslos!");
static const u8 sText_PkmnsXCuredItsYProblem[] = _("{STRING 23} von\n{STRING 16} heilte sein\lProblem mit {STRING 0}!");
static const u8 sText_PkmnsXHadNoEffectOnY[] = _("{STRING 23} von\n{STRING 16} hat keine\lWirkung auf {STRING 14}!");
static const u8 sText_StatSharply[] = _("steigt stark!");
const u8 gText_StatRose[] = _("steigt.");
static const u8 sText_StatHarshly[] = _("sinkt stark!");
static const u8 sText_StatFell[] = _("sinkt.");
static const u8 sText_AttackersStatRose[] = _("{STRING 0} von\n{EVIL_LEGENDARY} {PLAYER}");
const u8 gText_DefendersStatRose[] = _("{STRING 0} von\n{GOOD_LEGENDARY} {PLAYER}");
static const u8 sText_UsingItemTheStatOfPkmnRose[] = _("{STRING 19}: {STRING 0} von\n{STRING 16} {PLAYER}");
static const u8 sText_AttackersStatFell[] = _("{STRING 0} von\n{EVIL_LEGENDARY} {PLAYER}");
static const u8 sText_DefendersStatFell[] = _("{STRING 0} von\n{GOOD_LEGENDARY} {PLAYER}");
static const u8 sText_StatsWontIncrease2[] = _("Status von {EVIL_LEGENDARY}\nkann nicht weiter erhöht werden!");
static const u8 sText_StatsWontDecrease2[] = _("Status von {GOOD_LEGENDARY}\nkann nicht weiter sinken!");
static const u8 sText_CriticalHit[] = _("Ein Volltreffer!");
static const u8 sText_OneHitKO[] = _("Ein K.O.-Treffer!");
static const u8 sText_123Poof[] = _("{PAUSE 32}1, {PAUSE 15}2 und...{PAUSE 15} ...{PAUSE 15} ...{PAUSE 15}\n{PAUSE 15}{PLAY_SE 0x38 0x00}Schwupp!\p");
static const u8 sText_AndEllipsis[] = _("Und...\p");
static const u8 sText_HMMovesCantBeForgotten[] = _("VM-Attacken können jetzt \nnicht vergessen werden.\p");
static const u8 sText_NotVeryEffective[] = _("Das ist nicht sehr effektiv...");
static const u8 sText_SuperEffective[] = _("Das ist sehr effektiv!");
static const u8 sText_GotAwaySafely[] = _("{PLAY_SE 0x11 0x00}Du bist entkommen!\p");
static const u8 sText_PkmnFledUsingIts[] = _("{PLAY_SE 0x11 0x00}{EVIL_LEGENDARY} floh\ndurch Einsatz von {STRING 19}!\p");
static const u8 sText_PkmnFledUsing[] = _("{PLAY_SE 0x11 0x00}{EVIL_LEGENDARY} floh\ndurch Einsatz von {STRING 21}!\p");
static const u8 sText_WildPkmnFled[] = _("{PLAY_SE 0x11 0x00}Wildes {STRING 0} floh!");
static const u8 sText_PlayerDefeatedLinkTrainer[] = _("Spieler besiegte\n{STRING 29}!");
static const u8 sText_TwoLinkTrainersDefeated[] = _("Spieler besiegte\n{STRING 30} und {STRING 29}!");
static const u8 sText_PlayerLostAgainstLinkTrainer[] = _("Spieler verlor gegen\n{STRING 29}!");
static const u8 sText_PlayerLostToTwo[] = _("Spieler verlor gegen\n{STRING 30} und {STRING 29}!");
static const u8 sText_PlayerBattledToDrawLinkTrainer[] = _("Patt zwischen Spieler und\n{STRING 29}!");
static const u8 sText_PlayerBattledToDrawVsTwo[] = _("Patt zwischen Spieler, {STRING 30}\nund {STRING 29}!");
static const u8 sText_WildFled[] = _("{PLAY_SE 0x11 0x00}{STRING 29} floh!");
static const u8 sText_TwoWildFled[] = _("{PLAY_SE 0x11 0x00}{STRING 29} und\n{STRING 30} flohen!");
static const u8 sText_NoRunningFromTrainers[] = _("Du kannst aus TRAINER-Kämpfen\nnicht fliehen!\p");
static const u8 sText_CantEscape[] = _("Flucht unmöglich!\p");
static const u8 sText_DontLeaveBirch[] = _("PROF. BIRK: Du... Du kannst mich doch\nnicht einfach im Stich lassen!\p");
static const u8 sText_ButNothingHappened[] = _("Nichts geschieht!");
static const u8 sText_ButItFailed[] = _("Es schlug fehl!");
static const u8 sText_ItHurtConfusion[] = _("Es hat sich vor Verwirrung\nselbst verletzt!");
static const u8 sText_MirrorMoveFailed[] = _("Der SPIEGELTRICK schlug fehl!");
static const u8 sText_StartedToRain[] = _("Es fängt an zu regnen!");
static const u8 sText_DownpourStarted[] = _("Ein Wolkenbruch!");
static const u8 sText_RainContinues[] = _("Es regnet weiter.");
static const u8 sText_DownpourContinues[] = _("Der Wolkenbruch hält an.");
static const u8 sText_RainStopped[] = _("Es hat aufgehört zu regnen.");
static const u8 sText_SandstormBrewed[] = _("Ein Sandsturm kommt auf!");
static const u8 sText_SandstormRages[] = _("Der Sandsturm tobt.");
static const u8 sText_SandstormSubsided[] = _("Der Sandsturm legt sich.");
static const u8 sText_SunlightGotBright[] = _("Das Sonnenlicht wird stärker!");
static const u8 sText_SunlightStrong[] = _("Gleißendes Sonnenlicht!");
static const u8 sText_SunlightFaded[] = _("Das Sonnenlicht lässt nach!");
static const u8 sText_StartedHail[] = _("Es fängt an zu hageln!");
static const u8 sText_HailContinues[] = _("Es hagelt!");
static const u8 sText_HailStopped[] = _("Es hat aufgehört zu hageln.");
static const u8 sText_FailedToSpitUp[] = _("Aber es konnte nichts\nENTFESSELN!");
static const u8 sText_FailedToSwallow[] = _("Aber es konnte nichts\nVERZEHREN!");
static const u8 sText_WindBecameHeatWave[] = _("Der Wind wurde zu einer\nHITZEWELLE!");
static const u8 sText_StatChangesGone[] = _("Alle Statusveränderungen wurden\nentfernt!");
static const u8 sText_CoinsScattered[] = _("Es liegen überall Münzen verstreut!");
static const u8 sText_TooWeakForSubstitute[] = _("Zu schwach, um einen DELEGATOR\neinzusetzen!");
static const u8 sText_SharedPain[] = _("Die Kontrahenten teilen\nihr Leid!");
static const u8 sText_BellChimed[] = _("Eine Glocke läutet!");
static const u8 sText_FaintInThree[] = _("Alle betroffenen POKéMON werden\nin 3 Runden K.O. gehen!");
static const u8 sText_NoPPLeft[] = _("Es sind keine AP mehr für\ndiese Attacke übrig!\p");
static const u8 sText_ButNoPPLeft[] = _("Aber es waren keine AP mehr\nfür diese Attacke übrig!");
static const u8 sText_PkmnIgnoresAsleep[] = _("{EVIL_LEGENDARY} ignoriert die\nBefehle. Es schläft!");
static const u8 sText_PkmnIgnoredOrders[] = _("{EVIL_LEGENDARY} ignoriert den\nBefehl!");
static const u8 sText_PkmnBeganToNap[] = _("{EVIL_LEGENDARY} macht\nein Nickerchen!");
static const u8 sText_PkmnLoafing[] = _("{EVIL_LEGENDARY} faulenzt!");
static const u8 sText_PkmnWontObey[] = _("{EVIL_LEGENDARY} ist\nungehorsam!");
static const u8 sText_PkmnTurnedAway[] = _("{EVIL_LEGENDARY} wendet\nsich ab!");
static const u8 sText_PkmnPretendNotNotice[] = _("{EVIL_LEGENDARY} gibt vor,\nnichts zu bemerken!");
static const u8 sText_EnemyAboutToSwitchPkmn[] = _("Als Nächstes wird {PLAYER} von\n{STRING 25} {STRING 26} eingesetzt.\pWird {STRING 32} das\nPOKéMON wechseln?");
static const u8 sText_PkmnLearnedMove2[] = _("{EVIL_LEGENDARY} hat\n{STRING 0} gelernt!");
static const u8 sText_PlayerDefeatedLinkTrainerTrainer1[] = _("Spieler besiegte\n{STRING 25} {STRING 26}!\p");
static const u8 sText_CreptCloser[] = _("{STRING 32} schleicht sich näher an\n{STR_VAR_2} heran!");
static const u8 sText_CantGetCloser[] = _("{STRING 32} kann nicht näher herangehen!");
static const u8 sText_PkmnWatchingCarefully[] = _("{STR_VAR_2} beobachtet\nalles aufmerksam!");
static const u8 sText_PkmnCuriousAboutX[] = _("{STR_VAR_2} ist\nneugierig auf {STRING 0}!");
static const u8 sText_PkmnEnthralledByX[] = _("{STR_VAR_2} ist begeistert\nvon {STRING 0}!");
static const u8 sText_PkmnIgnoredX[] = _("{STR_VAR_2} ignoriert\n{STRING 0} völlig!");
static const u8 sText_ThrewPokeblockAtPkmn[] = _("{STRING 32} wirft {STR_VAR_2} einen\n{POKEBLOCK} zu!");
static const u8 sText_OutOfSafariBalls[] = _("{PLAY_SE 0x49 0x00}ANSAGE: Du hast keine\nSAFARIBÄLLE mehr! Game over!\p");
static const u8 sText_WildPkmnAppeared[] = _("Ein wildes {STR_VAR_2} erscheint!\p");
static const u8 sText_LegendaryPkmnAppeared[] = _("Ein wildes {STR_VAR_2} erscheint!\p");
static const u8 sText_WildPkmnAppearedPause[] = _("Ein wildes {STR_VAR_2} erscheint!{PAUSE 127}");
static const u8 sText_TwoWildPkmnAppeared[] = _("Ein wildes {KUN} und\nein wildes {STR_VAR_2} erscheinen!\p");
static const u8 sText_Trainer1WantsToBattle[] = _("Eine Herausforderung von\n{STRING 25} {STRING 26}!\p");
static const u8 sText_LinkTrainerWantsToBattle[] = _("Eine Herausforderung von\n{STRING 29}!");
static const u8 sText_TwoLinkTrainersWantToBattle[] = _("{STRING 29} und {STRING 30}\nmöchten kämpfen!");
static const u8 sText_Trainer1SentOutPkmn[] = _("{STR_VAR_2} wird von\n{STRING 25} {STRING 26} in den\lKampf geschickt!");
static const u8 sText_Trainer1SentOutTwoPkmn[] = _("{STR_VAR_2} und {KUN}\nwerden von\p{STRING 25} {STRING 26}\nin den Kampf geschickt!");
static const u8 sText_Trainer1SentOutPkmn2[] = _("{STRING 0} wird von\n{STRING 25} {STRING 26}\lin den Kampf geschickt!");
static const u8 sText_LinkTrainerSentOutPkmn[] = _("{STRING 29} schickt\n{STR_VAR_2} in den Kampf!");
static const u8 sText_LinkTrainerSentOutTwoPkmn[] = _("{STRING 29} schickt\n{STR_VAR_2} und {KUN}!");
static const u8 sText_TwoLinkTrainersSentOutPkmn[] = _("{STRING 29} schickt\n{VERSION}!\p{STRING 30} schickt\n{GOOD_TEAM}!");
static const u8 sText_LinkTrainerSentOutPkmn2[] = _("{STRING 29} schickt\n{STRING 0}!");
static const u8 sText_LinkTrainerMultiSentOutPkmn[] = _("{STRING 31} schickt\n{STRING 0}!");
static const u8 sText_GoPkmn[] = _("Los! {STR_VAR_1}!");
static const u8 sText_GoTwoPkmn[] = _("Los! {STR_VAR_1} und\n{STR_VAR_3}!");
static const u8 sText_GoPkmn2[] = _("Los! {STRING 0}!");
static const u8 sText_DoItPkmn[] = _("Du schaffst es! {STRING 0}!");
static const u8 sText_GoForItPkmn[] = _("Streng dich an, {STRING 0}!");
static const u8 sText_YourFoesWeakGetEmPkmn[] = _("Mach es fertig!\nLos, {STRING 0}!");
static const u8 sText_LinkPartnerSentOutPkmnGoPkmn[] = _("{STRING 28} schickt\n{EVIL_TEAM}!\pLos! {RIVAL}!");
static const u8 sText_PkmnThatsEnough[] = _("{STRING 0}, genug!\nKomm zurück!");
static const u8 sText_PkmnComeBack[] = _("{STRING 0}, komm zurück!");
static const u8 sText_PkmnOkComeBack[] = _("{STRING 0}, O.K.!\nKomm zurück!");
static const u8 sText_PkmnGoodComeBack[] = _("{STRING 0}, gut!\nKomm zurück!");
static const u8 sText_Trainer1WithdrewPkmn[] = _("{STRING 0} wurde\nvon {STRING 25} {STRING 26}\lzurückgerufen!");
static const u8 sText_LinkTrainer1WithdrewPkmn[] = _("{STRING 29} ruft\n{STRING 0} zurück!");
static const u8 sText_LinkTrainer2WithdrewPkmn[] = _("{STRING 31} ruft\n{STRING 0} zurück!");
static const u8 sText_WildPkmnPrefix[] = _(" (Wild)");
static const u8 sText_FoePkmnPrefix[] = _(" (Gegner)");
static const u8 sText_FoePkmnPrefix2[] = _("Gegner");
static const u8 sText_AllyPkmnPrefix[] = _("Anwender");
static const u8 sText_FoePkmnPrefix3[] = _("Gegner");
static const u8 sText_AllyPkmnPrefix2[] = _("Anwender");
static const u8 sText_FoePkmnPrefix4[] = _("Gegner");
static const u8 sText_AllyPkmnPrefix3[] = _("Anwender");
static const u8 sText_AttackerUsedX[] = _("{EVIL_LEGENDARY} setzt\n{PLAYER} ein!");
static const u8 sText_ExclamationMark[] = _("");
static const u8 sText_ExclamationMark2[] = _("");
static const u8 sText_ExclamationMark3[] = _("");
static const u8 sText_ExclamationMark4[] = _("");
static const u8 sText_ExclamationMark5[] = _("");
static const u8 sText_HP2[] = _("KP");
static const u8 sText_Attack2[] = _("ANGRIFF");
static const u8 sText_Defense2[] = _("VERT.");
static const u8 sText_Speed[] = _("INIT.");
static const u8 sText_SpAtk2[] = _("SP.ANG.");
static const u8 sText_SpDef2[] = _("SP.VER.");
static const u8 sText_Accuracy[] = _("GENAUIGKEIT");
static const u8 sText_Evasiveness[] = _("FLUCHTWERT");
static const u8 sText_PokeblockWasTooSpicy[] = _("zu scharf!");
static const u8 sText_PokeblockWasTooDry[] = _("zu trocken!");
static const u8 sText_PokeblockWasTooSweet[] = _("zu süß!");
static const u8 sText_PokeblockWasTooBitter[] = _("zu bitter!");
static const u8 sText_PokeblockWasTooSour[] = _("zu sauer!");
static const u8 sText_PlayerUsedItem[] = _("{STRING 32} setzt\n{STRING 19} ein!");
static const u8 sText_WallyUsedItem[] = _("HEIKO setzt\n{STRING 19} ein!");
static const u8 sText_Trainer1UsedItem[] = _("{STRING 19} wird von\n{STRING 25} {STRING 26} eingesetzt!");
static const u8 sText_TrainerBlockedBall[] = _("Der TRAINER hat den BALL abgeblockt!");
static const u8 sText_DontBeAThief[] = _("Sei kein Dieb!");
static const u8 sText_ItDodgedBall[] = _("Es ist dem BALL ausgewichen! Dieses\nPOKéMON kann nicht gefangen werden!");
static const u8 sText_YouMissedPkmn[] = _("Du hast das POKéMON verfehlt!");
static const u8 sText_PkmnBrokeFree[] = _("Mist!\nDas POKéMON hat sich befreit!");
static const u8 sText_ItAppearedCaught[] = _("Oh!\nFast hätte es geklappt!");
static const u8 sText_AarghAlmostHadIt[] = _("Mist!\nDas war knapp!");
static const u8 sText_ShootSoClose[] = _("Verflixt!\nEs hätte beinahe geklappt!");
static const u8 sText_GotchaPkmnCaughtPlayer[] = _("Toll!\n{STR_VAR_2} wurde gefangen!{UNKNOWN_A}{PLAY_BGM 0x60 0x01}\p");
static const u8 sText_GotchaPkmnCaughtWally[] = _("Toll!\n{STR_VAR_2} wurde gefangen!{UNKNOWN_A}{PLAY_BGM 0x60 0x01}{PAUSE 127}");
static const u8 sText_GiveNicknameCaptured[] = _("Möchtest du dem {STR_VAR_2}\neinen Spitznamen geben?");
static const u8 sText_PkmnSentToPC[] = _("{STR_VAR_2} wurde auf\n{STRING 35} PC übertragen.");
static const u8 sText_Someones[] = _("einen");
static const u8 sText_Lanettes[] = _("LANETTES");
static const u8 sText_PkmnDataAddedToDex[] = _("Für {STR_VAR_2} wurde ein Eintrag\nim POKéDEX angelegt.\p");
static const u8 sText_ItIsRaining[] = _("Es regnet.");
static const u8 sText_SandstormIsRaging[] = _("Ein Sandsturm tobt.");
static const u8 sText_BoxIsFull[] = _("Die BOXEN sind voll!\nDu kannst keines mehr fangen!\p");
static const u8 sText_EnigmaBerry[] = _("ENIGMABEERE");
static const u8 sText_BerrySuffix[] = _("{STR_VAR_1}BEERE");
static const u8 sText_PkmnsItemCuredParalysis[] = _("{STRING 19} von\n{STRING 16}\lheilte die Paralyse!");
static const u8 sText_PkmnsItemCuredPoison[] = _("{STRING 19} von\n{STRING 16}\lheilte die Vergiftung!");
static const u8 sText_PkmnsItemHealedBurn[] = _("{STRING 19} von\n{STRING 16}\lheilte die Verbrennung!");
static const u8 sText_PkmnsItemDefrostedIt[] = _("{STRING 19} von\n{STRING 16} taute es auf!");
static const u8 sText_PkmnsItemWokeIt[] = _("{STRING 19} von\n{STRING 16} \lweckte es auf!");
static const u8 sText_PkmnsItemSnappedOut[] = _("{STRING 19} von\n{STRING 16}\lhebt die Verwirrung auf!");
static const u8 sText_PkmnsItemCuredProblem[] = _("{STRING 19} von\n{STRING 16} \lheilte sein {STRING 0}-Problem!");
static const u8 sText_PkmnsItemNormalizedStatus[] = _("{STRING 19} von\n{STRING 16} normalisierte\lseine Statuswerte!");
static const u8 sText_PkmnsItemRestoredHealth[] = _("{STRING 19} füllte\nKP von {STRING 16} auf!");
static const u8 sText_PkmnsItemRestoredPP[] = _("{STRING 19} von\n{STRING 16} füllte AP von\l{STRING 0} auf!");
static const u8 sText_PkmnsItemRestoredStatus[] = _("{STRING 19} von\n{STRING 16}\pstellte seine\nStatuswerte wieder her!");
static const u8 sText_PkmnsItemRestoredHPALittle[] = _("{STRING 19} von\n{STRING 16}\lfüllte einige KP auf!");
static const u8 sText_ItemAllowsOnlyYMove[] = _("{STRING 19} erlaubt\nnur den Einsatz von {STRING 17}!\p");
static const u8 sText_PkmnHungOnWithX[] = _("{GOOD_LEGENDARY} hält\nmit Hilfe von\l{STRING 19} durch!");
const u8 gText_EmptyString3[] = _("");
static const u8 sText_YouThrowABallNowRight[] = _("Nun muss man einen BALL werfen, oder?\nIch... Ich werde mein Bestes geben!");
const u8 gText_PkmnIsEvolving[] = _("Hey?\n{STR_VAR_1} entwickelt sich!");
const u8 gText_CongratsPkmnEvolved[] = _("Glückwunsch! Dein {STR_VAR_1}\nwurde zu {STR_VAR_2}!{UNKNOWN_A}\p");
const u8 gText_PkmnStoppedEvolving[] = _("Hm? Die Entwicklung\nwurde abgebrochen!\p");
const u8 gText_WhatWillPkmnDo[] = _("Was soll\n{STRING 15} tun?");
const u8 gText_WhatWillPkmnDo2[] = _("Was wird {STRING 32}\ntun?");
const u8 gText_WhatWillWallyDo[] = _("Was wird\nHEIKO tun?");
const u8 gText_LinkStandby[] = _("{PAUSE 16}Verbindung...");
const u8 gText_BattleMenu[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}KAMPF{CLEAR_TO 46}BEUTEL\nPOKéMON{CLEAR_TO 46}FLUCHT");
const u8 gText_SafariZoneMenu[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}BALL{CLEAR_TO 46}{POKEBLOCK}\nNÄHER{CLEAR_TO 46}FLUCHT");
const u8 gText_MoveInterfacePPType[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}AP\nTYP/");
const u8 gText_MoveInterfaceDynamicColors[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}");
const u8 gText_WhichMoveToForget4[] = _("{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}{PALETTE 5}Welche Attacke soll vergessen werden?");
const u8 gText_BattleYesNoChoice[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}Ja\nNein");
const u8 gText_BattleSwitchWhich[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}\nWohin?");
const u8 gText_BattleSwitchWhich2[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}");
const u8 gText_RightArrow[] = _("{RIGHT_ARROW}");
const u8 gText_Plus[] = _("+");
const u8 gText_Dash[] = _("-");
static const u8 sText_HP[] = _("KP     ");
static const u8 sText_Attack[] = _("ANGR. ");
static const u8 sText_Defense[] = _("VERT.");
static const u8 sText_SpAtk[] = _("SP.ANG.");
static const u8 sText_SpDef[] = _("SP.VER.");
const u8 gText_SafariBalls[] = _("{HIGHLIGHT RED}SAFARIBÄLLE");
const u8 gText_SafariBallLeft[] = _("{HIGHLIGHT RED}Übrig: ");
const u8 BattleText_HighlightRed[] = _("{HIGHLIGHT RED}");
const u8 gText_Sleep[] = _("Schlaf");
const u8 gText_Poison[] = _("Gift");
const u8 gText_Burn[] = _("Verbrennung");
const u8 gText_Paralysis[] = _("Paralyse");
const u8 gText_Ice[] = _("Eis");
const u8 gText_Confusion[] = _("Verwirrung");
const u8 gText_Love[] = _("Liebe");
const u8 gText_SpaceAndSpace[] = _(" und ");
const u8 gText_CommaSpace[] = _(", ");
const u8 gText_Space2[] = _(" ");
const u8 gText_LineBreak[] = _("\l");
const u8 gText_NewLine[] = _("\n");
const u8 gText_Are[] = _("sind");
const u8 gText_Are2[] = _("sind");
const u8 gText_BadEgg[] = _("Schl. EI");
const u8 gText_BattleWallyName[] = _("HEIKO");
const u8 gText_Win[] = _("{HIGHLIGHT TRANSPARENT}Sieg");
const u8 gText_Loss[] = _("{HIGHLIGHT TRANSPARENT}Ndrl.");
const u8 gText_Draw[] = _("{HIGHLIGHT TRANSPARENT}Patt");
static const u8 sText_SpaceIs[] = _(" ist");
static const u8 sText_ApostropheS[] = _("");
const u8 BattleText_Normal[] = _("eine NORMALE Att.");
const u8 BattleText_Fighting[] = _("eine KAMPF-Att.");
const u8 BattleText_Flying[] = _("eine FLUG-Att.");
const u8 BattleText_Poison[] = _("eine GIFT-Att.");
const u8 BattleText_Ground[] = _("eine BODEN-Att.");
const u8 BattleText_Rock[] = _("eine GESTEINS-Att.");
const u8 BattleText_Bug[] = _("eine KÄFER-Att.");
const u8 BattleText_Ghost[] = _("eine GEIST-Att.");
const u8 BattleText_Steel[] = _("eine STAHL-Att.");
const u8 BattleText_Typeless[] = _("eine ???-Attacke");
const u8 BattleText_Fire[] = _("eine FEUER-Att.");
const u8 BattleText_Water[] = _("eine WASSER-Att.");
const u8 BattleText_Grass[] = _("eine PFLANZEN-Att.");
const u8 BattleText_Electric[] = _("eine ELEKTRO-Att.");
const u8 BattleText_Psychic[] = _("eine PSYCHO-Att.");
const u8 BattleText_Ice[] = _("eine EIS-Att.");
const u8 BattleText_Dragon[] = _("eine DRACHEN-Att.");
const u8 BattleText_Dark[] = _("eine UNLICHT-Att.");

const u8 *const gStatNamesTable[] =
{
    sText_HP2,
    sText_Attack2,
    sText_Defense2,
    sText_Speed,
    sText_SpAtk2,
    sText_SpDef2,
    sText_Accuracy,
    sText_Evasiveness,
};

const u8 *const gPokeblockWasTooXStringTable[] =
{
    sText_PokeblockWasTooSpicy,
    sText_PokeblockWasTooDry,
    sText_PokeblockWasTooSweet,
    sText_PokeblockWasTooBitter,
    sText_PokeblockWasTooSour,
};

const u8 *const gBattleStringsTable[] =
{
    sText_Trainer1LoseText,
    sText_PkmnGainedEXP,
    sText_PkmnGrewToLv,
    sText_PkmnLearnedMove,
    sText_TryToLearnMove1,
    sText_TryToLearnMove2,
    sText_TryToLearnMove3,
    sText_PkmnForgotMove,
    sText_StopLearningMove,
    sText_DidNotLearnMove,
    sText_PkmnLearnedMove2,
    sText_AttackMissed,
    sText_PkmnProtectedItself,
    sText_StatsWontIncrease2,
    sText_AvoidedDamage,
    sText_ItDoesntAffect,
    sText_AttackerFainted,
    sText_TargetFainted,
    sText_PlayerGotMoney,
    sText_PlayerWhiteout,
    sText_PlayerWhiteout2,
    sText_PreventsEscape,
    sText_HitXTimes,
    sText_PkmnFellAsleep,
    sText_PkmnMadeSleep,
    sText_PkmnAlreadyAsleep,
    sText_PkmnAlreadyAsleep2,
    sText_PkmnWasntAffected,
    sText_PkmnWasPoisoned,
    sText_PkmnPoisonedBy,
    sText_PkmnHurtByPoison,
    sText_PkmnAlreadyPoisoned,
    sText_PkmnBadlyPoisoned,
    sText_PkmnEnergyDrained,
    sText_PkmnWasBurned,
    sText_PkmnBurnedBy,
    sText_PkmnHurtByBurn,
    sText_PkmnWasFrozen,
    sText_PkmnFrozenBy,
    sText_PkmnIsFrozen,
    sText_PkmnWasDefrosted,
    sText_PkmnWasDefrosted2,
    sText_PkmnWasDefrostedBy,
    sText_PkmnWasParalyzed,
    sText_PkmnWasParalyzedBy,
    sText_PkmnIsParalyzed,
    sText_PkmnIsAlreadyParalyzed,
    sText_PkmnHealedParalysis,
    sText_PkmnDreamEaten,
    sText_StatsWontIncrease,
    sText_StatsWontDecrease,
    sText_TeamStoppedWorking,
    sText_FoeStoppedWorking,
    sText_PkmnIsConfused,
    sText_PkmnHealedConfusion,
    sText_PkmnWasConfused,
    sText_PkmnAlreadyConfused,
    sText_PkmnFellInLove,
    sText_PkmnInLove,
    sText_PkmnImmobilizedByLove,
    sText_PkmnBlownAway,
    sText_PkmnChangedType,
    sText_PkmnFlinched,
    sText_PkmnRegainedHealth,
    sText_PkmnHPFull,
    sText_PkmnRaisedSpDef,
    sText_PkmnRaisedDef,
    sText_PkmnCoveredByVeil,
    sText_PkmnUsedSafeguard,
    sText_PkmnSafeguardExpired,
    sText_PkmnWentToSleep,
    sText_PkmnSleptHealthy,
    sText_PkmnWhippedWhirlwind,
    sText_PkmnTookSunlight,
    sText_PkmnLoweredHead,
    sText_PkmnIsGlowing,
    sText_PkmnFlewHigh,
    sText_PkmnDugHole,
    sText_PkmnSqueezedByBind,
    sText_PkmnTrappedInVortex,
    sText_PkmnWrappedBy,
    sText_PkmnClamped,
    sText_PkmnHurtBy,
    sText_PkmnFreedFrom,
    sText_PkmnCrashed,
    gText_PkmnShroudedInMist,
    sText_PkmnProtectedByMist,
    gText_PkmnGettingPumped,
    sText_PkmnHitWithRecoil,
    sText_PkmnProtectedItself2,
    sText_PkmnBuffetedBySandstorm,
    sText_PkmnPeltedByHail,
    sText_PkmnSeeded,
    sText_PkmnEvadedAttack,
    sText_PkmnSappedByLeechSeed,
    sText_PkmnFastAsleep,
    sText_PkmnWokeUp,
    sText_PkmnUproarKeptAwake,
    sText_PkmnWokeUpInUproar,
    sText_PkmnCausedUproar,
    sText_PkmnMakingUproar,
    sText_PkmnCalmedDown,
    sText_PkmnCantSleepInUproar,
    sText_PkmnStockpiled,
    sText_PkmnCantStockpile,
    sText_PkmnCantSleepInUproar2,
    sText_UproarKeptPkmnAwake,
    sText_PkmnStayedAwakeUsing,
    sText_PkmnStoringEnergy,
    sText_PkmnUnleashedEnergy,
    sText_PkmnFatigueConfusion,
    sText_PlayerPickedUpMoney,
    sText_PkmnUnaffected,
    sText_PkmnTransformedInto,
    sText_PkmnMadeSubstitute,
    sText_PkmnHasSubstitute,
    sText_SubstituteDamaged,
    sText_PkmnSubstituteFaded,
    sText_PkmnMustRecharge,
    sText_PkmnRageBuilding,
    sText_PkmnMoveWasDisabled,
    sText_PkmnMoveIsDisabled,
    sText_PkmnMoveDisabledNoMore,
    sText_PkmnGotEncore,
    sText_PkmnEncoreEnded,
    sText_PkmnTookAim,
    sText_PkmnSketchedMove,
    sText_PkmnTryingToTakeFoe,
    sText_PkmnTookFoe,
    sText_PkmnReducedPP,
    sText_PkmnStoleItem,
    sText_TargetCantEscapeNow,
    sText_PkmnFellIntoNightmare,
    sText_PkmnLockedInNightmare,
    sText_PkmnLaidCurse,
    sText_PkmnAfflictedByCurse,
    sText_SpikesScattered,
    sText_PkmnHurtBySpikes,
    sText_PkmnIdentified,
    sText_PkmnPerishCountFell,
    sText_PkmnBracedItself,
    sText_PkmnEnduredHit,
    sText_MagnitudeStrength,
    sText_PkmnCutHPMaxedAttack,
    sText_PkmnCopiedStatChanges,
    sText_PkmnGotFree,
    sText_PkmnShedLeechSeed,
    sText_PkmnBlewAwaySpikes,
    sText_PkmnFledFromBattle,
    sText_PkmnForesawAttack,
    sText_PkmnTookAttack,
    sText_PkmnAttack,
    sText_PkmnCenterAttention,
    sText_PkmnChargingPower,
    sText_NaturePowerTurnedInto,
    sText_PkmnStatusNormal,
    sText_PkmnHasNoMovesLeft,
    sText_PkmnSubjectedToTorment,
    sText_PkmnCantUseMoveTorment,
    sText_PkmnTighteningFocus,
    sText_PkmnFellForTaunt,
    sText_PkmnCantUseMoveTaunt,
    sText_PkmnReadyToHelp,
    sText_PkmnSwitchedItems,
    sText_PkmnCopiedFoe,
    sText_PkmnMadeWish,
    sText_PkmnWishCameTrue,
    sText_PkmnPlantedRoots,
    sText_PkmnAbsorbedNutrients,
    sText_PkmnAnchoredItself,
    sText_PkmnWasMadeDrowsy,
    sText_PkmnKnockedOff,
    sText_PkmnSwappedAbilities,
    sText_PkmnSealedOpponentMove,
    sText_PkmnCantUseMoveSealed,
    sText_PkmnWantsGrudge,
    sText_PkmnLostPPGrudge,
    sText_PkmnShroudedItself,
    sText_PkmnMoveBounced,
    sText_PkmnWaitsForTarget,
    sText_PkmnSnatchedMove,
    sText_PkmnMadeItRain,
    sText_PkmnRaisedSpeed,
    sText_PkmnProtectedBy,
    sText_PkmnPreventsUsage,
    sText_PkmnRestoredHPUsing,
    sText_PkmnChangedTypeWith,
    sText_PkmnPreventsParalysisWith,
    sText_PkmnPreventsRomanceWith,
    sText_PkmnPreventsPoisoningWith,
    sText_PkmnPreventsConfusionWith,
    sText_PkmnRaisedFirePowerWith,
    sText_PkmnAnchorsItselfWith,
    sText_PkmnCutsAttackWith,
    sText_PkmnPreventsStatLossWith,
    sText_PkmnHurtsWith,
    sText_PkmnTraced,
    sText_StatSharply,
    gText_StatRose,
    sText_StatHarshly,
    sText_StatFell,
    sText_AttackersStatRose,
    gText_DefendersStatRose,
    sText_AttackersStatFell,
    sText_DefendersStatFell,
    sText_CriticalHit,
    sText_OneHitKO,
    sText_123Poof,
    sText_AndEllipsis,
    sText_NotVeryEffective,
    sText_SuperEffective,
    sText_GotAwaySafely,
    sText_WildPkmnFled,
    sText_NoRunningFromTrainers,
    sText_CantEscape,
    sText_DontLeaveBirch,
    sText_ButNothingHappened,
    sText_ButItFailed,
    sText_ItHurtConfusion,
    sText_MirrorMoveFailed,
    sText_StartedToRain,
    sText_DownpourStarted,
    sText_RainContinues,
    sText_DownpourContinues,
    sText_RainStopped,
    sText_SandstormBrewed,
    sText_SandstormRages,
    sText_SandstormSubsided,
    sText_SunlightGotBright,
    sText_SunlightStrong,
    sText_SunlightFaded,
    sText_StartedHail,
    sText_HailContinues,
    sText_HailStopped,
    sText_FailedToSpitUp,
    sText_FailedToSwallow,
    sText_WindBecameHeatWave,
    sText_StatChangesGone,
    sText_CoinsScattered,
    sText_TooWeakForSubstitute,
    sText_SharedPain,
    sText_BellChimed,
    sText_FaintInThree,
    sText_NoPPLeft,
    sText_ButNoPPLeft,
    sText_PlayerUsedItem,
    sText_WallyUsedItem,
    sText_TrainerBlockedBall,
    sText_DontBeAThief,
    sText_ItDodgedBall,
    sText_YouMissedPkmn,
    sText_PkmnBrokeFree,
    sText_ItAppearedCaught,
    sText_AarghAlmostHadIt,
    sText_ShootSoClose,
    sText_GotchaPkmnCaughtPlayer,
    sText_GotchaPkmnCaughtWally,
    sText_GiveNicknameCaptured,
    sText_PkmnSentToPC,
    sText_PkmnDataAddedToDex,
    sText_ItIsRaining,
    sText_SandstormIsRaging,
    sText_CantEscape2,
    sText_PkmnIgnoresAsleep,
    sText_PkmnIgnoredOrders,
    sText_PkmnBeganToNap,
    sText_PkmnLoafing,
    sText_PkmnWontObey,
    sText_PkmnTurnedAway,
    sText_PkmnPretendNotNotice,
    sText_EnemyAboutToSwitchPkmn,
    sText_CreptCloser,
    sText_CantGetCloser,
    sText_PkmnWatchingCarefully,
    sText_PkmnCuriousAboutX,
    sText_PkmnEnthralledByX,
    sText_PkmnIgnoredX,
    sText_ThrewPokeblockAtPkmn,
    sText_OutOfSafariBalls,
    sText_PkmnsItemCuredParalysis,
    sText_PkmnsItemCuredPoison,
    sText_PkmnsItemHealedBurn,
    sText_PkmnsItemDefrostedIt,
    sText_PkmnsItemWokeIt,
    sText_PkmnsItemSnappedOut,
    sText_PkmnsItemCuredProblem,
    sText_PkmnsItemRestoredHealth,
    sText_PkmnsItemRestoredPP,
    sText_PkmnsItemRestoredStatus,
    sText_PkmnsItemRestoredHPALittle,
    sText_ItemAllowsOnlyYMove,
    sText_PkmnHungOnWithX,
    gText_EmptyString3,
    sText_PkmnsXPreventsBurns,
    sText_PkmnsXBlocksY,
    sText_PkmnsXRestoredHPALittle2,
    sText_PkmnsXWhippedUpSandstorm,
    sText_PkmnsXPreventsYLoss,
    sText_PkmnsXInfatuatedY,
    sText_PkmnsXMadeYIneffective,
    sText_PkmnsXCuredYProblem,
    sText_ItSuckedLiquidOoze,
    sText_PkmnTransformed,
    sText_ElectricityWeakened,
    sText_FireWeakened,
    sText_PkmnHidUnderwater,
    sText_PkmnSprangUp,
    sText_HMMovesCantBeForgotten,
    sText_XFoundOneY,
    sText_PlayerDefeatedLinkTrainerTrainer1,
    sText_SoothingAroma,
    sText_ItemsCantBeUsedNow,
    sText_ForXCommaYZ,
    sText_UsingItemTheStatOfPkmnRose,
    sText_PkmnUsedXToGetPumped,
    sText_PkmnsXMadeYUseless,
    sText_PkmnTrappedBySandTomb,
    sText_EmptyString4,
    sText_ABoosted,
    sText_PkmnsXIntensifiedSun,
    sText_PkmnMakesGroundMiss,
    sText_YouThrowABallNowRight,
    sText_PkmnsXTookAttack,
    sText_PkmnChoseXAsDestiny,
    sText_PkmnLostFocus,
    sText_UseNextPkmn,
    sText_PkmnFledUsingIts,
    sText_PkmnFledUsing,
    sText_PkmnWasDraggedOut,
    sText_PreventedFromWorking,
    sText_PkmnsItemNormalizedStatus,
    sText_Trainer1UsedItem,
    sText_BoxIsFull,
    sText_PkmnAvoidedAttack,
    sText_PkmnsXMadeItIneffective,
    sText_PkmnsXPreventsFlinching,
    sText_PkmnAlreadyHasBurn,
    sText_StatsWontDecrease2,
    sText_PkmnsXBlocksY2,
    sText_PkmnsXWoreOff,
    sText_PkmnRaisedDefALittle,
    sText_PkmnRaisedSpDefALittle,
    sText_TheWallShattered,
    sText_PkmnsXPreventsYsZ,
    sText_PkmnsXCuredItsYProblem,
    sText_AttackerCantEscape,
    sText_PkmnObtainedX,
    sText_PkmnObtainedX2,
    sText_PkmnObtainedXYObtainedZ,
    sText_ButNoEffect,
    sText_PkmnsXHadNoEffectOnY,
};

// below are lists of battle string enums and NOT pointers to the strings.
const u16 gMissStringIds[] =
{
    STRINGID_ATTACKMISSED,
    STRINGID_PKMNPROTECTEDITSELF,
    STRINGID_PKMNAVOIDEDATTACK,
    STRINGID_AVOIDEDDAMAGE,
    STRINGID_PKMNMAKESGROUNDMISS,
};

const u16 gNoEscapeStringIds[] =
{
    STRINGID_CANTESCAPE,
    STRINGID_DONTLEAVEBIRCH,
    STRINGID_PREVENTSESCAPE,
    STRINGID_CANTESCAPE2,
    STRINGID_ATTACKERCANTESCAPE,
};

const u16 gMoveWeatherChangeStringIds[] =
{
    STRINGID_STARTEDTORAIN,
    STRINGID_DOWNPOURSTARTED,
    STRINGID_BUTITFAILED,
    STRINGID_SANDSTORMBREWED,
    STRINGID_SUNLIGHTGOTBRIGHT,
    STRINGID_STARTEDHAIL,
};

const u16 gSandStormHailContinuesStringIds[] =
{
    STRINGID_SANDSTORMRAGES,
    STRINGID_HAILCONTINUES,
};

const u16 gSandStormHailDmgStringIds[] =
{
    STRINGID_PKMNBUFFETEDBYSANDSTORM,
    STRINGID_PKMNPELTEDBYHAIL,
};

const u16 gSandStormHailEndStringIds[] =
{
    STRINGID_SANDSTORMSUBSIDED,
    STRINGID_HAILSTOPPED,
};

const u16 gRainContinuesStringIds[] =
{
    STRINGID_RAINCONTINUES,
    STRINGID_DOWNPOURCONTINUES,
    STRINGID_RAINSTOPPED,
};

const u16 gProtectLikeUsedStringIds[] =
{
    STRINGID_PKMNPROTECTEDITSELF2,
    STRINGID_PKMNBRACEDITSELF,
    STRINGID_BUTITFAILED,
};

const u16 gReflectLightScreenSafeguardStringIds[] =
{
    STRINGID_BUTITFAILED,
    STRINGID_PKMNRAISEDDEF,
    STRINGID_PKMNRAISEDDEFALITTLE,
    STRINGID_PKMNRAISEDSPDEF,
    STRINGID_PKMNRAISEDSPDEFALITTLE,
    STRINGID_PKMNCOVEREDBYVEIL,
};

const u16 gLeechSeedStringIds[] =
{
    STRINGID_PKMNSEEDED,
    STRINGID_PKMNEVADEDATTACK,
    STRINGID_ITDOESNTAFFECT,
    STRINGID_PKMNSAPPEDBYLEECHSEED,
    STRINGID_ITSUCKEDLIQUIDOOZE,
};

const u16 gRestUsedStringIds[] =
{
    STRINGID_PKMNWENTTOSLEEP,
    STRINGID_PKMNSLEPTHEALTHY,
};

const u16 gUproarOverTurnStringIds[] =
{
    STRINGID_PKMNMAKINGUPROAR,
    STRINGID_PKMNCALMEDDOWN,
};

const u16 gStockpileUsedStringIds[] =
{
    STRINGID_PKMNSTOCKPILED,
    STRINGID_PKMNCANTSTOCKPILE,
};

const u16 gWokeUpStringIds[] =
{
    STRINGID_PKMNWOKEUP,
    STRINGID_PKMNWOKEUPINUPROAR,
};

const u16 gSwallowFailStringIds[] =
{
    STRINGID_FAILEDTOSWALLOW,
    STRINGID_PKMNHPFULL,
};

const u16 gUproarAwakeStringIds[] =
{
    STRINGID_PKMNCANTSLEEPINUPROAR2,
    STRINGID_UPROARKEPTPKMNAWAKE,
    STRINGID_PKMNSTAYEDAWAKEUSING,
};

const u16 gStatUpStringIds[] =
{
    STRINGID_ATTACKERSSTATROSE,
    STRINGID_DEFENDERSSTATROSE,
    STRINGID_STATSWONTINCREASE,
    STRINGID_EMPTYSTRING3,
    STRINGID_USINGITEMSTATOFPKMNROSE,
    STRINGID_PKMNUSEDXTOGETPUMPED,
};

const u16 gStatDownStringIds[] =
{
    STRINGID_ATTACKERSSTATFELL,
    STRINGID_DEFENDERSSTATFELL,
    STRINGID_STATSWONTDECREASE,
    STRINGID_EMPTYSTRING3,
};

const u16 gFirstTurnOfTwoStringIds[] =
{
    STRINGID_PKMNWHIPPEDWHIRLWIND,
    STRINGID_PKMNTOOKSUNLIGHT,
    STRINGID_PKMNLOWEREDHEAD,
    STRINGID_PKMNISGLOWING,
    STRINGID_PKMNFLEWHIGH,
    STRINGID_PKMNDUGHOLE,
    STRINGID_PKMNHIDUNDERWATER,
    STRINGID_PKMNSPRANGUP,
};

const u16 gWrappedStringIds[] =
{
    STRINGID_PKMNSQUEEZEDBYBIND,
    STRINGID_PKMNWRAPPEDBY,
    STRINGID_PKMNTRAPPEDINVORTEX,
    STRINGID_PKMNCLAMPED,
    STRINGID_PKMNTRAPPEDINVORTEX,
    STRINGID_PKMNTRAPPEDBYSANDTOMB,
};

const u16 gMistUsedStringIds[] =
{
    STRINGID_PKMNSHROUDEDINMIST,
    STRINGID_BUTITFAILED,
};

const u16 gFocusEnergyUsedStringIds[] =
{
    STRINGID_PKMNGETTINGPUMPED,
    STRINGID_BUTITFAILED,
};

const u16 gTransformUsedStringIds[] =
{
    STRINGID_PKMNTRANSFORMEDINTO,
    STRINGID_BUTITFAILED,
};

const u16 gSubstituteUsedStringIds[] =
{
    STRINGID_PKMNMADESUBSTITUTE,
    STRINGID_TOOWEAKFORSUBSTITUTE,
};

const u16 gGotPoisonedStringIds[] =
{
    STRINGID_PKMNWASPOISONED,
    STRINGID_PKMNPOISONEDBY,
};

const u16 gGotParalyzedStringIds[] =
{
    STRINGID_PKMNWASPARALYZED,
    STRINGID_PKMNWASPARALYZEDBY,
};

const u16 gFellAsleepStringIds[] =
{
    STRINGID_PKMNFELLASLEEP,
    STRINGID_PKMNMADESLEEP,
};

const u16 gGotBurnedStringIds[] =
{
    STRINGID_PKMNWASBURNED,
    STRINGID_PKMNBURNEDBY,
};

const u16 gGotFrozenStringIds[] =
{
    STRINGID_PKMNWASFROZEN,
    STRINGID_PKMNFROZENBY,
};

const u16 gGotDefrostedStringIds[] =
{
    STRINGID_PKMNWASDEFROSTED2,
    STRINGID_PKMNWASDEFROSTEDBY,
};

const u16 gKOFailedStringIds[] =
{
    STRINGID_ATTACKMISSED,
    STRINGID_PKMNUNAFFECTED,
    STRINGID_PKMNFELLINLOVE,
    STRINGID_PKMNSXINFATUATEDY,
};

const u16 gAbsorbDrainStringIds[] =
{
    STRINGID_PKMNENERGYDRAINED,
    STRINGID_ITSUCKEDLIQUIDOOZE,
};

const u16 gSportsUsedStringIds[] =
{
    STRINGID_ELECTRICITYWEAKENED,
    STRINGID_FIREWEAKENED,
};

const u16 gPartyStatusHealStringIds[] =
{
    STRINGID_BELLCHIMED,
    STRINGID_BELLCHIMED,
    STRINGID_BELLCHIMED,
    STRINGID_BELLCHIMED,
    STRINGID_SOOTHINGAROMA,
};

const u16 gFutureMoveUsedStringIds[] =
{
    STRINGID_PKMNFORESAWATTACK,
    STRINGID_PKMNCHOSEXASDESTINY,
};

const u16 gBallEscapeStringIds[] =
{
    STRINGID_PKMNBROKEFREE,
    STRINGID_ITAPPEAREDCAUGHT,
    STRINGID_AARGHALMOSTHADIT,
    STRINGID_SHOOTSOCLOSE,
};

const u16 gWeatherStartsStringIds[] =
{
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_SANDSTORMISRAGING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_SUNLIGHTSTRONG,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
    STRINGID_ITISRAINING,
};

const u16 gInobedientStringIds[] =
{
    STRINGID_PKMNLOAFING,
    STRINGID_PKMNWONTOBEY,
    STRINGID_PKMNTURNEDAWAY,
    STRINGID_PKMNPRETENDNOTNOTICE,
};

const u16 gSafariGetNearStringIds[] =
{
    STRINGID_CREPTCLOSER,
    STRINGID_CANTGETCLOSER,
};

const u16 gSafariPokeblockResultStringIds[] =
{
    STRINGID_PKMNCURIOUSABOUTX,
    STRINGID_PKMNENTHRALLEDBYX,
    STRINGID_PKMNIGNOREDX,
};

const u16 gTrainerItemCuredStatusStringIds[] =
{
    STRINGID_PKMNSITEMSNAPPEDOUT,
    STRINGID_PKMNSITEMCUREDPARALYSIS,
    STRINGID_PKMNSITEMDEFROSTEDIT,
    STRINGID_PKMNSITEMHEALEDBURN,
    STRINGID_PKMNSITEMCUREDPOISON,
    STRINGID_PKMNSITEMWOKEIT,
};

const u16 gBerryEffectStringIds[] =
{
    STRINGID_PKMNSITEMCUREDPROBLEM,
    STRINGID_PKMNSITEMNORMALIZEDSTATUS,
};

const u16 gBRNPreventionStringIds[] =
{
    STRINGID_PKMNSXPREVENTSBURNS,
    STRINGID_PKMNSXPREVENTSYSZ,
    STRINGID_PKMNSXHADNOEFFECTONY,
};

const u16 gPRLZPreventionStringIds[] =
{
    STRINGID_PKMNPREVENTSPARALYSISWITH,
    STRINGID_PKMNSXPREVENTSYSZ,
    STRINGID_PKMNSXHADNOEFFECTONY,
};

const u16 gPSNPreventionStringIds[] =
{
    STRINGID_PKMNPREVENTSPOISONINGWITH,
    STRINGID_PKMNSXPREVENTSYSZ,
    STRINGID_PKMNSXHADNOEFFECTONY,
};

const u16 gItemSwapStringIds[] =
{
    STRINGID_PKMNOBTAINEDX,
    STRINGID_PKMNOBTAINEDX2,
    STRINGID_PKMNOBTAINEDXYOBTAINEDZ,
};

const u16 gFlashFireStringIds[] =
{
    STRINGID_PKMNRAISEDFIREPOWERWITH,
    STRINGID_PKMNSXMADEYINEFFECTIVE,
};

const u16 gTrappingMoves[] =
{
    MOVE_BIND,
    MOVE_WRAP,
    MOVE_FIRE_SPIN,
    MOVE_CLAMP,
    MOVE_WHIRLPOOL,
    MOVE_SAND_TOMB,
    0xFFFF,
};

const u8 *const gStatNamesTable2[] =
{
    sText_HP,
    sText_SpAtk,
    sText_Attack,
    sText_SpDef,
    sText_Defense,
    sText_Speed,
};

static const u8 *const sATypeMove_Table[] =
{
    BattleText_Normal,
    BattleText_Fighting,
    BattleText_Flying,
    BattleText_Poison,
    BattleText_Ground,
    BattleText_Rock,
    BattleText_Bug,
    BattleText_Ghost,
    BattleText_Steel,
    BattleText_Typeless,
    BattleText_Fire,
    BattleText_Water,
    BattleText_Grass,
    BattleText_Electric,
    BattleText_Psychic,
    BattleText_Ice,
    BattleText_Dragon,
    BattleText_Dark,
};
