#include "constants/moves.h"

enum
{
    BATTLE_TEXT_UnknownString = 12,
    BATTLE_TEXT_GainExpPoints,
    BATTLE_TEXT_GrewLevel,
    BATTLE_TEXT_LearnedMove,
    BATTLE_TEXT_TryingToLearnMove,
    BATTLE_TEXT_CantLearnMore,
    BATTLE_TEXT_DeleteMove,
    BATTLE_TEXT_DeletedMove,
    BATTLE_TEXT_StopLearning,
    BATTLE_TEXT_DidNotLearn,
    BATTLE_TEXT_LearnedMove2,
    BATTLE_TEXT_AttackMissed,
    BATTLE_TEXT_ProtectedItself,
    BATTLE_TEXT_StatNoHigher,
    BATTLE_TEXT_AvoidedDamage,
    BATTLE_TEXT_DoesntAffect,
    BATTLE_TEXT_AttackingFainted,
    BATTLE_TEXT_DefendingFainted,
    BATTLE_TEXT_WinningPrize,
    BATTLE_TEXT_OutOfUsablePoke,
    BATTLE_TEXT_WhitedOut,
    BATTLE_TEXT_PreventEscape,
    BATTLE_TEXT_HitMulti,
    BATTLE_TEXT_FellAsleep,
    BATTLE_TEXT_MadeAsleep,
    BATTLE_TEXT_DefendingAsleep,
    BATTLE_TEXT_AttackingAsleep,
    BATTLE_TEXT_WasntAffected,
    BATTLE_TEXT_Poisoned,
    BATTLE_TEXT_Poisoned2,
    BATTLE_TEXT_PoisonHurt,
    BATTLE_TEXT_AlreadyPoisoned,
    BATTLE_TEXT_BadlyPoisoned,
    BATTLE_TEXT_EnergyDrained,
    BATTLE_TEXT_Burned,
    BATTLE_TEXT_Burned2,
    BATTLE_TEXT_BurnHurt,
    BATTLE_TEXT_Frozen,
    BATTLE_TEXT_Frozen2,
    BATTLE_TEXT_FrozenSolid,
    BATTLE_TEXT_DefendingDefrosted,
    BATTLE_TEXT_AttackingDefrosted,
    BATTLE_TEXT_Defrosted,
    BATTLE_TEXT_Paralyzed,
    BATTLE_TEXT_Paralyzed2,
    BATTLE_TEXT_Paralyzed3,
    BATTLE_TEXT_AlreadyParalyzed,
    BATTLE_TEXT_ParalysisHealed,
    BATTLE_TEXT_DreamEaten,
    BATTLE_TEXT_AttackingStatNoHigher,
    BATTLE_TEXT_DefendingStatNoHigher,
    BATTLE_TEXT_StoppedWorking,
    BATTLE_TEXT_StoppedWorking2,
    BATTLE_TEXT_Confused,
    BATTLE_TEXT_ConfusionSnapOut,
    BATTLE_TEXT_BecameConfused,
    BATTLE_TEXT_AlreadyConfused,
    BATTLE_TEXT_FellLove,
    BATTLE_TEXT_InLoveWith,
    BATTLE_TEXT_ImmobilizedBy,
    BATTLE_TEXT_BlownAway,
    BATTLE_TEXT_TypeTransform,
    BATTLE_TEXT_Flinched,
    BATTLE_TEXT_RegainedHealth,
    BATTLE_TEXT_HPFull,
    BATTLE_TEXT_RaisedSpDef,
    BATTLE_TEXT_RaisedDefense,
    BATTLE_TEXT_CoveredVeil,
    BATTLE_TEXT_SafeguardActive,
    BATTLE_TEXT_SafeguardFaded,
    BATTLE_TEXT_WentToSleep,
    BATTLE_TEXT_SpeltHealthy,
    BATTLE_TEXT_WhipWhirlwind,
    BATTLE_TEXT_TookSunlight,
    BATTLE_TEXT_LoweredHead,
    BATTLE_TEXT_IsGlowing,
    BATTLE_TEXT_FlewHigh,
    BATTLE_TEXT_DugHole,
    BATTLE_TEXT_SqueezedBind,
    BATTLE_TEXT_TrappedVortex,
    BATTLE_TEXT_Wrapped,
    BATTLE_TEXT_Clamped,
    BATTLE_TEXT_HurtBy,
    BATTLE_TEXT_FreedFrom,
    BATTLE_TEXT_KeptGoingCrash,
    BATTLE_TEXT_MistShroud,
    BATTLE_TEXT_MistProtect,
    BATTLE_TEXT_GetPumped,
    BATTLE_TEXT_HitRecoil,
    BATTLE_TEXT_ProtectedItself2,
    BATTLE_TEXT_SandBuffeted,
    BATTLE_TEXT_HailStricken,
    BATTLE_TEXT_WasSeeded,
    BATTLE_TEXT_EvadedAttack,
    BATTLE_TEXT_HealthSapped,
    BATTLE_TEXT_FastAsleep,
    BATTLE_TEXT_WokeUp,
    BATTLE_TEXT_UproarAwake,
    BATTLE_TEXT_UproarWoke,
    BATTLE_TEXT_UproarCaused,
    BATTLE_TEXT_UproarMaking,
    BATTLE_TEXT_CalmedDown,
    BATTLE_TEXT_UproarCantSleep,
    BATTLE_TEXT_Stockpiled,
    BATTLE_TEXT_StockpiledCant,
    BATTLE_TEXT_UproarCantSleep2,
    BATTLE_TEXT_UproarKeptAwake,
    BATTLE_TEXT_StayedAwake,
    BATTLE_TEXT_StoringEnergy,
    BATTLE_TEXT_UnleashedEnergy,
    BATTLE_TEXT_FatigueConfuse,
    BATTLE_TEXT_PickedUpYen,
    BATTLE_TEXT_Unaffected,
    BATTLE_TEXT_Transformed,
    BATTLE_TEXT_SubMade,
    BATTLE_TEXT_SubAlready,
    BATTLE_TEXT_SubTookDamage,
    BATTLE_TEXT_SubFaded,
    BATTLE_TEXT_MustRecharge,
    BATTLE_TEXT_RageBuilding,
    BATTLE_TEXT_MoveWasDisabled,
    BATTLE_TEXT_MoveIsDisabled,
    BATTLE_TEXT_DisabledNoMore,
    BATTLE_TEXT_EncoreGot,
    BATTLE_TEXT_EncoreEnded,
    BATTLE_TEXT_TookAim,
    BATTLE_TEXT_SketchedMove,
    BATTLE_TEXT_DestinyBondTake,
    BATTLE_TEXT_DestinyBondTaken,
    BATTLE_TEXT_ReducedBy,
    BATTLE_TEXT_StoleSomething,
    BATTLE_TEXT_CantEscapeNow,
    BATTLE_TEXT_NightmareStart,
    BATTLE_TEXT_NightmareLock,
    BATTLE_TEXT_CurseLay,
    BATTLE_TEXT_CurseAfflict,
    BATTLE_TEXT_SpikesScattered,
    BATTLE_TEXT_SpikesHurt,
    BATTLE_TEXT_IdentifiedPoke,
    BATTLE_TEXT_PerishSongFell,
    BATTLE_TEXT_BracedItself,
    BATTLE_TEXT_EnduredHit,
    BATTLE_TEXT_MagnitudeCount,
    BATTLE_TEXT_CutHPMaxATK,
    BATTLE_TEXT_CopyStatChanges,
    BATTLE_TEXT_GotFreeFrom,
    BATTLE_TEXT_LeechShed,
    BATTLE_TEXT_SpikesBlownAway,
    BATTLE_TEXT_FledBattle,
    BATTLE_TEXT_ForesawAttack,
    BATTLE_TEXT_TookAttack,
    BATTLE_TEXT_PokeAttack,
    BATTLE_TEXT_CenterAttention,
    BATTLE_TEXT_ChargingPower,
    BATTLE_TEXT_NaturePower,
    BATTLE_TEXT_StatusNormal,
    BATTLE_TEXT_NoMovesLeft,
    BATTLE_TEXT_TormentSubject,
    BATTLE_TEXT_TormentNoUse,
    BATTLE_TEXT_TightenFocus,
    BATTLE_TEXT_TauntFell,
    BATTLE_TEXT_TauntNoUse,
    BATTLE_TEXT_ReadyToHelp,
    BATTLE_TEXT_SwitchedItems,
    BATTLE_TEXT_CopiedObject,
    BATTLE_TEXT_WishMade,
    BATTLE_TEXT_WishTrue,
    BATTLE_TEXT_PlantedRoots,
    BATTLE_TEXT_AbsorbNutrients,
    BATTLE_TEXT_AnchoredItself,
    BATTLE_TEXT_DrowsyMade,
    BATTLE_TEXT_KnockedOffItem,
    BATTLE_TEXT_AbilitySwap,
    BATTLE_TEXT_SealedMove,
    BATTLE_TEXT_SealedNoUse,
    BATTLE_TEXT_GrudgeBear,
    BATTLE_TEXT_GrudgeLosePP,
    BATTLE_TEXT_ShroudedItself,
    BATTLE_TEXT_MagicCoatBounce,
    BATTLE_TEXT_AwaitMove,
    BATTLE_TEXT_SnatchedMove,
    BATTLE_TEXT_RainMade,
    BATTLE_TEXT_SpeedRisen,
    BATTLE_TEXT_ProtectedBy,
    BATTLE_TEXT_PreventedBy,
    BATTLE_TEXT_HPRestoredUsing,
    BATTLE_TEXT_MadeType,
    BATTLE_TEXT_PreventedPara,
    BATTLE_TEXT_PreventedRomance,
    BATTLE_TEXT_PreventedPoison,
    BATTLE_TEXT_PreventedConfusion,
    BATTLE_TEXT_RaisedFirePower,
    BATTLE_TEXT_AnchorsItself,
    BATTLE_TEXT_CutsAttack,
    BATTLE_TEXT_PreventedStatLoss,
    BATTLE_TEXT_HurtOther,
    BATTLE_TEXT_Traced,
    BATTLE_TEXT_Sharply,
    BATTLE_TEXT_Rose,
    BATTLE_TEXT_Harshly,
    BATTLE_TEXT_Fell,
    BATTLE_TEXT_UnknownString7,
    BATTLE_TEXT_UnknownString3,
    BATTLE_TEXT_UnknownString5,
    BATTLE_TEXT_UnknownString6,
    BATTLE_TEXT_Critical,
    BATTLE_TEXT_GrandSlam,
    BATTLE_TEXT_MoveForget123,
    BATTLE_TEXT_MoveForgetAnd,
    BATTLE_TEXT_NotEffective,
    BATTLE_TEXT_SuperEffective,
    BATTLE_TEXT_GotAwaySafely,
    BATTLE_TEXT_FledWild,
    BATTLE_TEXT_NoRunning,
    BATTLE_TEXT_CantEscape2,
    BATTLE_TEXT_BirchDontLeaveMe,
    BATTLE_TEXT_Nothing,
    BATTLE_TEXT_Failed,
    BATTLE_TEXT_HurtItself,
    BATTLE_TEXT_MirrorFail,
    BATTLE_TEXT_RainStart,
    BATTLE_TEXT_PourStart,
    BATTLE_TEXT_RainCont1,
    BATTLE_TEXT_PourCont,
    BATTLE_TEXT_RainStop,
    BATTLE_TEXT_SandBrewed,
    BATTLE_TEXT_SandRages,
    BATTLE_TEXT_SandSubsided,
    BATTLE_TEXT_SunBright,
    BATTLE_TEXT_SunStrong,
    BATTLE_TEXT_SunFaded,
    BATTLE_TEXT_HailStart,
    BATTLE_TEXT_HailCont,
    BATTLE_TEXT_HailStop,
    BATTLE_TEXT_SpitUpFail,
    BATTLE_TEXT_SwallowFail,
    BATTLE_TEXT_WindHeatWave,
    BATTLE_TEXT_StatElim,
    BATTLE_TEXT_CoinScatter,
    BATTLE_TEXT_SubWeak,
    BATTLE_TEXT_PainSplit,
    BATTLE_TEXT_BellChime,
    BATTLE_TEXT_PerishSong,
    BATTLE_TEXT_NoPP1,
    BATTLE_TEXT_NoPP2,
    BATTLE_TEXT_Used1,
    BATTLE_TEXT_TutorialUsed,
    BATTLE_TEXT_BlockBall,
    BATTLE_TEXT_DontBeAThief,
    BATTLE_TEXT_DodgeBall,
    BATTLE_TEXT_MissPoke,
    BATTLE_TEXT_BallBrokeOhNo,
    BATTLE_TEXT_BallBrokeAppeared,
    BATTLE_TEXT_BallBrokeAlmost,
    BATTLE_TEXT_BallBrokeSoClose,
    BATTLE_TEXT_BallCaught1,
    BATTLE_TEXT_BallCaught2,
    BATTLE_TEXT_GiveNickname,
    BATTLE_TEXT_SentToPC,
    BATTLE_TEXT_AddedToDex,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Sandstorm,
    BATTLE_TEXT_CantEscape,
    BATTLE_TEXT_IgnoredOrdersSLP,
    BATTLE_TEXT_IgnoredOrders,
    BATTLE_TEXT_BeganNap,
    BATTLE_TEXT_LoafingAround,
    BATTLE_TEXT_WontObey,
    BATTLE_TEXT_TurnedAway,
    BATTLE_TEXT_NotNotice,
    BATTLE_TEXT_WillSwitch,
    BATTLE_TEXT_CreptCloser,
    BATTLE_TEXT_CantGetCloser,
    BATTLE_TEXT_WatchingCarefully,
    BATTLE_TEXT_CuriousAbout,
    BATTLE_TEXT_EnthralledBy,
    BATTLE_TEXT_IgnoredThing,
    BATTLE_TEXT_ThrewBlock,
    BATTLE_TEXT_SafariOver,
    BATTLE_TEXT_CuredParalysis,
    BATTLE_TEXT_CuredPoison,
    BATTLE_TEXT_CuredBurn,
    BATTLE_TEXT_CuredFreeze,
    BATTLE_TEXT_CuredSleep,
    BATTLE_TEXT_CuredConfusion,
    BATTLE_TEXT_CuredStatus,
    BATTLE_TEXT_RestoredHealth,
    BATTLE_TEXT_RestoredPP,
    BATTLE_TEXT_RestoredStatus,
    BATTLE_TEXT_RestoredHPLittle,
    BATTLE_TEXT_ChoiceBand,
    BATTLE_TEXT_FocusSash,
    BATTLE_TEXT_Terminator2,
    BATTLE_TEXT_PreventedBurn,
    BATTLE_TEXT_BlocksOther,
    BATTLE_TEXT_RestoredHPByItem,
    BATTLE_TEXT_WhipSandstorm,
    BATTLE_TEXT_PreventedLoss,
    BATTLE_TEXT_InfatuatedPoke,
    BATTLE_TEXT_MadeIneffective,
    BATTLE_TEXT_CuredProblem,
    BATTLE_TEXT_OozeSuckup,
    BATTLE_TEXT_Transformed2,
    BATTLE_TEXT_ElecWeakened,
    BATTLE_TEXT_FireWeakened,
    BATTLE_TEXT_HidUnderwater,
    BATTLE_TEXT_SprangUp,
    BATTLE_TEXT_CantForgetHM,
    BATTLE_TEXT_FoundOne,
    BATTLE_TEXT_PlayerDefeatedTrainer2,
    BATTLE_TEXT_SoothingAroma,
    BATTLE_TEXT_CantUseItems,
    BATTLE_TEXT_UnknownString2,
    BATTLE_TEXT_UnknownString4,
    BATTLE_TEXT_HustleUse,
    BATTLE_TEXT_MadeUseless,
    BATTLE_TEXT_SandTombTrapped,
    BATTLE_TEXT_Terminator,
    BATTLE_TEXT_BoostedExp,
    BATTLE_TEXT_SunIntensified,
    BATTLE_TEXT_GroundMoveNegate,
    BATTLE_TEXT_WallyBall,
    BATTLE_TEXT_TookAttack2,
    BATTLE_TEXT_ChoseDestiny,
    BATTLE_TEXT_LostFocus,
    BATTLE_TEXT_UseNext,
    BATTLE_TEXT_FledUsingItem,
    BATTLE_TEXT_FledUsingOther,
    BATTLE_TEXT_DraggedOut,
    BATTLE_TEXT_PreventedOther,
    BATTLE_TEXT_NormalizedStatus,
    BATTLE_TEXT_Used2,
    BATTLE_TEXT_BoxFull,
    BATTLE_TEXT_AvoidedAttack,
    BATTLE_TEXT_MadeIneffective2,
    BATTLE_TEXT_PreventedFlinching,
    BATTLE_TEXT_AlreadyBurned,
    BATTLE_TEXT_StatNoLower,
    BATTLE_TEXT_BlocksOther2,
    BATTLE_TEXT_WoreOff,
    BATTLE_TEXT_RaisedDefenseLittle,
    BATTLE_TEXT_RaisedSpDefLittle,
    BATTLE_TEXT_BrokeWall,
    BATTLE_TEXT_PreventedOther2,
    BATTLE_TEXT_CuredOfProblem,
    BATTLE_TEXT_AttackingCantEscape,
    BATTLE_TEXT_Obtained1,
    BATTLE_TEXT_Obtained2,
    BATTLE_TEXT_Obtained3,
    BATTLE_TEXT_NoEffect,
    BATTLE_TEXT_NoEffectOn,
};

static const u8 sText_Trainer1LoseText[] = _("{STRING 33}");
static const u8 sText_PkmnGainedEXP[] = _("{STRING 0} gained{STRING 1}\n{STRING 42} EXP. Points!\p");
static const u8 sText_EmptyString4[] = _("");
static const u8 sText_ABoosted[] = _(" a boosted");
static const u8 sText_PkmnGrewToLv[] = _("{STRING 0} grew to\nLV. {STRING 1}!{UNKNOWN_A}\p");
static const u8 sText_PkmnLearnedMove[] = _("{STRING 0} learned\n{STRING 1}!{UNKNOWN_A}\p");
static const u8 sText_TryToLearnMove1[] = _("{STRING 0} is trying to\nlearn {STRING 1}.\p");
static const u8 sText_TryToLearnMove2[] = _("But, {STRING 0} can't learn\nmore than four moves.\p");
static const u8 sText_TryToLearnMove3[] = _("Delete a move to make\nroom for {STRING 1}?");
static const u8 sText_PkmnForgotMove[] = _("{STRING 0} forgot\n{STRING 1}.\p");
static const u8 sText_StopLearningMove[] = _("{PAUSE 32}Stop learning\n{STRING 1}?");
static const u8 sText_DidNotLearnMove[] = _("{STRING 0} did not learn\n{STRING 1}.\p");
static const u8 sText_UseNextPkmn[] = _("Use next POKéMON?");
static const u8 sText_AttackMissed[] = _("{ATTACKING_MON}'s\nattack missed!");
static const u8 sText_PkmnProtectedItself[] = _("{DEFENDING_MON}\nprotected itself!");
static const u8 sText_AvoidedDamage[] = _("{DEFENDING_MON} avoided\ndamage with {STRING 22}!");
static const u8 sText_PkmnMakesGroundMiss[] = _("{DEFENDING_MON} makes GROUND\nmoves miss with {STRING 22}!");
static const u8 sText_PkmnAvoidedAttack[] = _("{DEFENDING_MON} avoided\nthe attack!");
static const u8 sText_ItDoesntAffect[] = _("It doesn't affect\n{DEFENDING_MON}...");
static const u8 sText_AttackerFainted[] = _("{ATTACKING_MON}\nfainted!\p");
static const u8 sText_TargetFainted[] = _("{DEFENDING_MON}\nfainted!\p");
static const u8 sText_PlayerGotMoney[] = _("{STRING 32} got ¥{STRING 0}\nfor winning!\p");
static const u8 sText_PlayerWhiteout[] = _("{STRING 32} is out of\nuseable POKéMON!\p");
static const u8 sText_PlayerWhiteout2[] = _("{STRING 32} whited out!{PAUSE_UNTIL_PRESS}");
static const u8 sText_PreventsEscape[] = _("{STRING 16} prevents\nescape with {STRING 23}!\p");
static const u8 sText_CantEscape2[] = _("Can't escape!\p");
static const u8 sText_AttackerCantEscape[] = _("{ATTACKING_MON}\ncan't escape!");
static const u8 sText_HitXTimes[] = _("Hit {STRING 0} time(s)!");
static const u8 sText_PkmnFellAsleep[] = _("{STRING 14}\nfell asleep!");
static const u8 sText_PkmnMadeSleep[] = _("{STRING 16}'s {STRING 23}\nmade {STRING 14} sleep!");
static const u8 sText_PkmnAlreadyAsleep[] = _("{DEFENDING_MON} is\nalready asleep!");
static const u8 sText_PkmnAlreadyAsleep2[] = _("{ATTACKING_MON} is\nalready asleep!");
static const u8 sText_PkmnWasntAffected[] = _("{DEFENDING_MON}\nwasn't affected!");
static const u8 sText_PkmnWasPoisoned[] = _("{STRING 14}\nwas poisoned!");
static const u8 sText_PkmnPoisonedBy[] = _("{STRING 16}'s {STRING 23}\npoisoned {STRING 14}!");
static const u8 sText_PkmnHurtByPoison[] = _("{ATTACKING_MON} is hurt\nby poison!");
static const u8 sText_PkmnAlreadyPoisoned[] = _("{DEFENDING_MON} is already\npoisoned.");
static const u8 sText_PkmnBadlyPoisoned[] = _("{STRING 14} is badly\npoisoned!");
static const u8 sText_PkmnEnergyDrained[] = _("{DEFENDING_MON} had its\nenergy drained!");
static const u8 sText_PkmnWasBurned[] = _("{STRING 14} was burned!");
static const u8 sText_PkmnBurnedBy[] = _("{STRING 16}'s {STRING 23}\nburned {STRING 14}!");
static const u8 sText_PkmnHurtByBurn[] = _("{ATTACKING_MON} is hurt\nby its burn!");
static const u8 sText_PkmnAlreadyHasBurn[] = _("{DEFENDING_MON} already\nhas a burn.");
static const u8 sText_PkmnWasFrozen[] = _("{STRING 14} was\nfrozen solid!");
static const u8 sText_PkmnFrozenBy[] = _("{STRING 16}'s {STRING 23}\nfroze {STRING 14} solid!");
static const u8 sText_PkmnIsFrozen[] = _("{ATTACKING_MON} is\nfrozen solid!");
static const u8 sText_PkmnWasDefrosted[] = _("{DEFENDING_MON} was\ndefrosted!");
static const u8 sText_PkmnWasDefrosted2[] = _("{ATTACKING_MON} was\ndefrosted!");
static const u8 sText_PkmnWasDefrostedBy[] = _("{ATTACKING_MON} was\ndefrosted by {STRING 17}!");
static const u8 sText_PkmnWasParalyzed[] = _("{STRING 14} is paralyzed!\nIt may be unable to move!");
static const u8 sText_PkmnWasParalyzedBy[] = _("{STRING 16}'s {STRING 23}\nparalyzed {STRING 14}!\lIt may be unable to move!");
static const u8 sText_PkmnIsParalyzed[] = _("{ATTACKING_MON} is paralyzed!\nIt can't move!");
static const u8 sText_PkmnIsAlreadyParalyzed[] = _("{DEFENDING_MON} is\nalready paralyzed!");
static const u8 sText_PkmnHealedParalysis[] = _("{DEFENDING_MON} was\nhealed of paralysis!");
static const u8 sText_PkmnDreamEaten[] = _("{DEFENDING_MON}'s\ndream was eaten!");
static const u8 sText_StatsWontIncrease[] = _("{ATTACKING_MON}'s {STRING 0}\nwon't go higher!");
static const u8 sText_StatsWontDecrease[] = _("{DEFENDING_MON}'s {STRING 0}\nwon't go lower!");
static const u8 sText_TeamStoppedWorking[] = _("Your team's {STRING 0}\nstopped working!");
static const u8 sText_FoeStoppedWorking[] = _("The foe's {STRING 0}\nstopped working!");
static const u8 sText_PkmnIsConfused[] = _("{ATTACKING_MON} is\nconfused!");
static const u8 sText_PkmnHealedConfusion[] = _("{ATTACKING_MON} snapped\nout of confusion!");
static const u8 sText_PkmnWasConfused[] = _("{STRING 14} became\nconfused!");
static const u8 sText_PkmnAlreadyConfused[] = _("{DEFENDING_MON} is\nalready confused!");
static const u8 sText_PkmnFellInLove[] = _("{DEFENDING_MON}\nfell in love!");
static const u8 sText_PkmnInLove[] = _("{ATTACKING_MON} is in love\nwith {STRING 16}!");
static const u8 sText_PkmnImmobilizedByLove[] = _("{ATTACKING_MON} is\nimmobilized by love!");
static const u8 sText_PkmnBlownAway[] = _("{DEFENDING_MON} was\nblown away!");
static const u8 sText_PkmnChangedType[] = _("{ATTACKING_MON} transformed\ninto the {STRING 0} type!");
static const u8 sText_PkmnFlinched[] = _("{ATTACKING_MON} flinched!");
static const u8 sText_PkmnRegainedHealth[] = _("{DEFENDING_MON} regained\nhealth!");
static const u8 sText_PkmnHPFull[] = _("{DEFENDING_MON}'s\nHP is full!");
static const u8 sText_PkmnRaisedSpDef[] = _("{STRING 38}'s {STRING 17}\nraised SP. DEF!");
static const u8 sText_PkmnRaisedSpDefALittle[] = _("{STRING 38}'s {STRING 17}\nraised SP. DEF a little!");
static const u8 sText_PkmnRaisedDef[] = _("{STRING 38}'s {STRING 17}\nraised DEFENSE!");
static const u8 sText_PkmnRaisedDefALittle[] = _("{STRING 38}'s {STRING 17}\nraised DEFENSE a little!");
static const u8 sText_PkmnCoveredByVeil[] = _("{STRING 38} is covered\nby a veil!");
static const u8 sText_PkmnUsedSafeguard[] = _("{DEFENDING_MON} is protected\nby SAFEGUARD!");
static const u8 sText_PkmnSafeguardExpired[] = _("{STRING 40}'s SAFEGUARD\nfaded!");
static const u8 sText_PkmnWentToSleep[] = _("{ATTACKING_MON} went\nto sleep!");
static const u8 sText_PkmnSleptHealthy[] = _("{ATTACKING_MON} slept and\nbecame healthy!");
static const u8 sText_PkmnWhippedWhirlwind[] = _("{ATTACKING_MON} whipped\nup a whirlwind!");
static const u8 sText_PkmnTookSunlight[] = _("{ATTACKING_MON} took\nin sunlight!");
static const u8 sText_PkmnLoweredHead[] = _("{ATTACKING_MON} lowered\nits head!");
static const u8 sText_PkmnIsGlowing[] = _("{ATTACKING_MON} is glowing!");
static const u8 sText_PkmnFlewHigh[] = _("{ATTACKING_MON} flew\nup high!");
static const u8 sText_PkmnDugHole[] = _("{ATTACKING_MON} dug a hole!");
static const u8 sText_PkmnHidUnderwater[] = _("{ATTACKING_MON} hid\nunderwater!");
static const u8 sText_PkmnSprangUp[] = _("{ATTACKING_MON} sprang up!");
static const u8 sText_PkmnSqueezedByBind[] = _("{DEFENDING_MON} was squeezed by\n{ATTACKING_MON}'s BIND!");
static const u8 sText_PkmnTrappedInVortex[] = _("{DEFENDING_MON} was trapped\nin the vortex!");
static const u8 sText_PkmnTrappedBySandTomb[] = _("{DEFENDING_MON} was trapped\nby SAND TOMB!");
static const u8 sText_PkmnWrappedBy[] = _("{DEFENDING_MON} was WRAPPED by\n{ATTACKING_MON}!");
static const u8 sText_PkmnClamped[] = _("{ATTACKING_MON} CLAMPED\n{DEFENDING_MON}!");
static const u8 sText_PkmnHurtBy[] = _("{ATTACKING_MON} is hurt\nby {STRING 0}!");
static const u8 sText_PkmnFreedFrom[] = _("{ATTACKING_MON} was freed\nfrom {STRING 0}!");
static const u8 sText_PkmnCrashed[] = _("{ATTACKING_MON} kept going\nand crashed!");
const u8 gText_PkmnShroudedInMist[] = _("{STRING 38} became\nshrouded in MIST!");
static const u8 sText_PkmnProtectedByMist[] = _("{STRING 16} is protected\nby MIST!");
const u8 gText_PkmnGettingPumped[] = _("{ATTACKING_MON} is getting\npumped!");
static const u8 sText_PkmnHitWithRecoil[] = _("{ATTACKING_MON} is hit\nwith recoil!");
static const u8 sText_PkmnProtectedItself2[] = _("{ATTACKING_MON} protected\nitself!");
static const u8 sText_PkmnBuffetedBySandstorm[] = _("{ATTACKING_MON} is buffeted\nby the sandstorm!");
static const u8 sText_PkmnPeltedByHail[] = _("{ATTACKING_MON} is stricken\nby HAIL!");
static const u8 sText_PkmnsXWoreOff[] = _("{STRING 36}'s {STRING 0}\nwore off!");
static const u8 sText_PkmnSeeded[] = _("{DEFENDING_MON} was seeded!");
static const u8 sText_PkmnEvadedAttack[] = _("{DEFENDING_MON} evaded\nthe attack!");
static const u8 sText_PkmnSappedByLeechSeed[] = _("{ATTACKING_MON}'s health is\nsapped by LEECH SEED!");
static const u8 sText_PkmnFastAsleep[] = _("{ATTACKING_MON} is fast\nasleep.");
static const u8 sText_PkmnWokeUp[] = _("{ATTACKING_MON} woke up!");
static const u8 sText_PkmnUproarKeptAwake[] = _("But {STRING 16}'s UPROAR\nkept it awake!");
static const u8 sText_PkmnWokeUpInUproar[] = _("{ATTACKING_MON} woke up\nin the UPROAR!");
static const u8 sText_PkmnCausedUproar[] = _("{ATTACKING_MON} caused\nan UPROAR!");
static const u8 sText_PkmnMakingUproar[] = _("{ATTACKING_MON} is making\nan UPROAR!");
static const u8 sText_PkmnCalmedDown[] = _("{ATTACKING_MON} calmed down.");
static const u8 sText_PkmnCantSleepInUproar[] = _("But {DEFENDING_MON} can't\nsleep in an UPROAR!");
static const u8 sText_PkmnStockpiled[] = _("{ATTACKING_MON} STOCKPILED\n{STRING 0}!");
static const u8 sText_PkmnCantStockpile[] = _("{ATTACKING_MON} can't\nSTOCKPILE any more!");
static const u8 sText_PkmnCantSleepInUproar2[] = _("But {DEFENDING_MON} can't\nsleep in an UPROAR!");
static const u8 sText_UproarKeptPkmnAwake[] = _("But the UPROAR kept\n{DEFENDING_MON} awake!");
static const u8 sText_PkmnStayedAwakeUsing[] = _("{DEFENDING_MON} stayed awake\nusing its {STRING 22}!");
static const u8 sText_PkmnStoringEnergy[] = _("{ATTACKING_MON} is storing\nenergy!");
static const u8 sText_PkmnUnleashedEnergy[] = _("{ATTACKING_MON} unleashed\nenergy!");
static const u8 sText_PkmnFatigueConfusion[] = _("{ATTACKING_MON} became\nconfused out of fatigue!");
static const u8 sText_PlayerPickedUpMoney[] = _("{STRING 32} picked up\n¥{STRING 0}!\p");
static const u8 sText_PkmnUnaffected[] = _("{DEFENDING_MON} is\nunaffected!");
static const u8 sText_PkmnTransformedInto[] = _("{ATTACKING_MON} transformed\ninto {STRING 0}!");
static const u8 sText_PkmnMadeSubstitute[] = _("{ATTACKING_MON} made\na SUBSTITUTE!");
static const u8 sText_PkmnHasSubstitute[] = _("{ATTACKING_MON} already\nhas a SUBSTITUTE!");
static const u8 sText_SubstituteDamaged[] = _("The SUBSTITUTE took damage\nfor {DEFENDING_MON}!\p");
static const u8 sText_PkmnSubstituteFaded[] = _("{DEFENDING_MON}'s\nSUBSTITUTE faded!\p");
static const u8 sText_PkmnMustRecharge[] = _("{ATTACKING_MON} must\nrecharge!");
static const u8 sText_PkmnRageBuilding[] = _("{DEFENDING_MON}'s RAGE\nis building!");
static const u8 sText_PkmnMoveWasDisabled[] = _("{DEFENDING_MON}'s {STRING 0}\nwas disabled!");
static const u8 sText_PkmnMoveDisabledNoMore[] = _("{ATTACKING_MON} is disabled\nno more!");
static const u8 sText_PkmnGotEncore[] = _("{DEFENDING_MON} got\nan ENCORE!");
static const u8 sText_PkmnEncoreEnded[] = _("{ATTACKING_MON}'s ENCORE\nended!");
static const u8 sText_PkmnTookAim[] = _("{ATTACKING_MON} took aim\nat {DEFENDING_MON}!");
static const u8 sText_PkmnSketchedMove[] = _("{ATTACKING_MON} SKETCHED\n{STRING 0}!");
static const u8 sText_PkmnTryingToTakeFoe[] = _("{ATTACKING_MON} is trying\nto take its foe with it!");
static const u8 sText_PkmnTookFoe[] = _("{DEFENDING_MON} took\n{ATTACKING_MON} with it!");
static const u8 sText_PkmnReducedPP[] = _("Reduced {DEFENDING_MON}'s\n{STRING 0} by {STRING 1}!");
static const u8 sText_PkmnStoleItem[] = _("{ATTACKING_MON} stole\n{DEFENDING_MON}'s {STRING 19}!");
static const u8 sText_TargetCantEscapeNow[] = _("{DEFENDING_MON} can't\nescape now!");
static const u8 sText_PkmnFellIntoNightmare[] = _("{DEFENDING_MON} fell into\na NIGHTMARE!");
static const u8 sText_PkmnLockedInNightmare[] = _("{ATTACKING_MON} is locked\nin a NIGHTMARE!");
static const u8 sText_PkmnLaidCurse[] = _("{ATTACKING_MON} cut its own HP\nand laid a CURSE on\l{DEFENDING_MON}!");
static const u8 sText_PkmnAfflictedByCurse[] = _("{ATTACKING_MON} is afflicted\nby the CURSE!");
static const u8 sText_SpikesScattered[] = _("{STRING 37} POKéMON team had SPIKES\nscattered around it!");
static const u8 sText_PkmnHurtBySpikes[] = _("{STRING 16} is hurt\nby SPIKES!");
static const u8 sText_PkmnIdentified[] = _("{ATTACKING_MON} identified\n{DEFENDING_MON}!");
static const u8 sText_PkmnPerishCountFell[] = _("{ATTACKING_MON}'s PERISH count\nfell to {STRING 0}!");
static const u8 sText_PkmnBracedItself[] = _("{ATTACKING_MON} braced\nitself!");
static const u8 sText_PkmnEnduredHit[] = _("{DEFENDING_MON} ENDURED\nthe hit!");
static const u8 sText_MagnitudeStrength[] = _("MAGNITUDE {STRING 0}!");
static const u8 sText_PkmnCutHPMaxedAttack[] = _("{ATTACKING_MON} cut its own HP\nand maximized ATTACK!");
static const u8 sText_PkmnCopiedStatChanges[] = _("{ATTACKING_MON} copied\n{DEFENDING_MON}'s stat changes!");
static const u8 sText_PkmnGotFree[] = _("{ATTACKING_MON} got free of\n{DEFENDING_MON}'s {STRING 0}!");
static const u8 sText_PkmnShedLeechSeed[] = _("{ATTACKING_MON} shed\nLEECH SEED!");
static const u8 sText_PkmnBlewAwaySpikes[] = _("{ATTACKING_MON} blew away\nSPIKES!");
static const u8 sText_PkmnFledFromBattle[] = _("{ATTACKING_MON} fled from\nbattle!");
static const u8 sText_PkmnForesawAttack[] = _("{ATTACKING_MON} foresaw\nan attack!");
static const u8 sText_PkmnTookAttack[] = _("{DEFENDING_MON} took\n{STRING 0}'s attack!");
static const u8 sText_PkmnChoseXAsDestiny[] = _("{ATTACKING_MON} chose\n{STRING 17} as its destiny!");
static const u8 sText_PkmnAttack[] = _("{STRING 0}'s attack!");
static const u8 sText_PkmnCenterAttention[] = _("{ATTACKING_MON} became the\ncenter of attention!");
static const u8 sText_PkmnChargingPower[] = _("{ATTACKING_MON} began\ncharging power!");
static const u8 sText_NaturePowerTurnedInto[] = _("NATURE POWER turned into\n{STRING 17}!");
static const u8 sText_PkmnStatusNormal[] = _("{ATTACKING_MON}'s status\nreturned to normal!");
static const u8 sText_PkmnSubjectedToTorment[] = _("{DEFENDING_MON} was subjected\nto TORMENT!");
static const u8 sText_PkmnTighteningFocus[] = _("{ATTACKING_MON} is tightening\nits focus!");
static const u8 sText_PkmnFellForTaunt[] = _("{DEFENDING_MON} fell for\nthe TAUNT!");
static const u8 sText_PkmnReadyToHelp[] = _("{ATTACKING_MON} is ready to\nhelp {DEFENDING_MON}!");
static const u8 sText_PkmnSwitchedItems[] = _("{ATTACKING_MON} switched\nitems with its opponent!");
static const u8 sText_PkmnObtainedX[] = _("{ATTACKING_MON} obtained\n{STRING 0}.");
static const u8 sText_PkmnObtainedX2[] = _("{DEFENDING_MON} obtained\n{STRING 1}.");
static const u8 sText_PkmnObtainedXYObtainedZ[] = _("{ATTACKING_MON} obtained\n{STRING 0}.\p{DEFENDING_MON} obtained\n{STRING 1}.");
static const u8 sText_PkmnCopiedFoe[] = _("{ATTACKING_MON} copied\n{DEFENDING_MON}'s {STRING 22}!");
static const u8 sText_PkmnMadeWish[] = _("{ATTACKING_MON} made a WISH!");
static const u8 sText_PkmnWishCameTrue[] = _("{STRING 0}'s WISH\ncame true!");
static const u8 sText_PkmnPlantedRoots[] = _("{ATTACKING_MON} planted its roots!");
static const u8 sText_PkmnAbsorbedNutrients[] = _("{ATTACKING_MON} absorbed\nnutrients with its roots!");
static const u8 sText_PkmnAnchoredItself[] = _("{DEFENDING_MON} anchored\nitself with its roots!");
static const u8 sText_PkmnWasMadeDrowsy[] = _("{ATTACKING_MON} made\n{DEFENDING_MON} drowsy!");
static const u8 sText_PkmnKnockedOff[] = _("{ATTACKING_MON} knocked off\n{DEFENDING_MON}'s {STRING 19}!");
static const u8 sText_PkmnSwappedAbilities[] = _("{ATTACKING_MON} swapped abilities\nwith its opponent!");
static const u8 sText_PkmnSealedOpponentMove[] = _("{ATTACKING_MON} sealed the\nopponent's move(s)!");
static const u8 sText_PkmnWantsGrudge[] = _("{ATTACKING_MON} wants the\nopponent to bear a GRUDGE!");
static const u8 sText_PkmnLostPPGrudge[] = _("{ATTACKING_MON}'s {STRING 0}\nlost all its PP due to\lthe GRUDGE!");
static const u8 sText_PkmnShroudedItself[] = _("{ATTACKING_MON} shrouded\nitself in {STRING 17}!");
static const u8 sText_PkmnMoveBounced[] = _("{ATTACKING_MON}'s {STRING 17}\nwas bounced back by MAGIC COAT!");
static const u8 sText_PkmnWaitsForTarget[] = _("{ATTACKING_MON} awaits its foe\nto make a move!");
static const u8 sText_PkmnSnatchedMove[] = _("{DEFENDING_MON} SNATCHED\n{STRING 16}'s move!");
static const u8 sText_ElectricityWeakened[] = _("Electricity's power was\nweakened!");
static const u8 sText_FireWeakened[] = _("Fire's power was\nweakened!");
static const u8 sText_XFoundOneY[] = _("{ATTACKING_MON} found\none {STRING 19}!");
static const u8 sText_SoothingAroma[] = _("A soothing aroma wafted\nthrough the area!");
static const u8 sText_ItemsCantBeUsedNow[] = _("Items can't be used now.{PAUSE 64}");
static const u8 sText_ForXCommaYZ[] = _("For {STRING 16},\n{STRING 19} {STRING 0}");
static const u8 sText_PkmnUsedXToGetPumped[] = _("{STRING 16} used\n{STRING 19} to hustle!");
static const u8 sText_PkmnLostFocus[] = _("{ATTACKING_MON} lost its\nfocus and couldn't move!");
static const u8 sText_PkmnWasDraggedOut[] = _("{DEFENDING_MON} was\ndragged out!\p");
static const u8 sText_TheWallShattered[] = _("The wall shattered!");
static const u8 sText_ButNoEffect[] = _("But it had no effect!");
static const u8 sText_PkmnHasNoMovesLeft[] = _("{STRING 15} has no\nmoves left!\p");
static const u8 sText_PkmnMoveIsDisabled[] = _("{STRING 15}'s {STRING 17}\nis disabled!\p");
static const u8 sText_PkmnCantUseMoveTorment[] = _("{STRING 15} can't use the same\nmove twice in a row due to the TORMENT!\p");
static const u8 sText_PkmnCantUseMoveTaunt[] = _("{STRING 15} can't use\n{STRING 17} after the TAUNT!\p");
static const u8 sText_PkmnCantUseMoveSealed[] = _("{STRING 15} can't use the\nsealed {STRING 17}!\p");
static const u8 sText_PkmnMadeItRain[] = _("{STRING 16}'s {STRING 23}\nmade it rain!");
static const u8 sText_PkmnRaisedSpeed[] = _("{STRING 16}'s {STRING 23}\nraised its SPEED!");
static const u8 sText_PkmnProtectedBy[] = _("{DEFENDING_MON} was protected\nby {STRING 22}!");
static const u8 sText_PkmnPreventsUsage[] = _("{DEFENDING_MON}'s {STRING 22}\nprevents {ATTACKING_MON}\lfrom using {STRING 17}!");
static const u8 sText_PkmnRestoredHPUsing[] = _("{DEFENDING_MON} restored HP\nusing its {STRING 22}!");
static const u8 sText_PkmnsXMadeYUseless[] = _("{DEFENDING_MON}'s {STRING 22}\nmade {STRING 17} useless!");
static const u8 sText_PkmnChangedTypeWith[] = _("{DEFENDING_MON}'s {STRING 22}\nmade it the {STRING 0} type!");
static const u8 sText_PkmnPreventsParalysisWith[] = _("{STRING 14}'s {STRING 22}\nprevents paralysis!");
static const u8 sText_PkmnPreventsRomanceWith[] = _("{DEFENDING_MON}'s {STRING 22}\nprevents romance!");
static const u8 sText_PkmnPreventsPoisoningWith[] = _("{STRING 14}'s {STRING 22}\nprevents poisoning!");
static const u8 sText_PkmnPreventsConfusionWith[] = _("{DEFENDING_MON}'s {STRING 22}\nprevents confusion!");
static const u8 sText_PkmnRaisedFirePowerWith[] = _("{DEFENDING_MON}'s {STRING 22}\nraised its FIRE power!");
static const u8 sText_PkmnAnchorsItselfWith[] = _("{DEFENDING_MON} anchors\nitself with {STRING 22}!");
static const u8 sText_PkmnCutsAttackWith[] = _("{STRING 16}'s {STRING 23}\ncuts {DEFENDING_MON}'s ATTACK!");
static const u8 sText_PkmnPreventsStatLossWith[] = _("{STRING 16}'s {STRING 23}\nprevents stat loss!");
static const u8 sText_PkmnHurtsWith[] = _("{DEFENDING_MON}'s {STRING 22}\nhurt {ATTACKING_MON}!");
static const u8 sText_PkmnTraced[] = _("{STRING 16} TRACED\n{STRING 0}'s {STRING 1}!");
static const u8 sText_PkmnsXPreventsBurns[] = _("{STRING 14}'s {STRING 24}\nprevents burns!");
static const u8 sText_PkmnsXBlocksY[] = _("{DEFENDING_MON}'s {STRING 22}\nblocks {STRING 17}!");
static const u8 sText_PkmnsXBlocksY2[] = _("{STRING 16}'s {STRING 23}\nblocks {STRING 17}!");
static const u8 sText_PkmnsXRestoredHPALittle2[] = _("{ATTACKING_MON}'s {STRING 21}\nrestored some HP!");
static const u8 sText_PkmnsXWhippedUpSandstorm[] = _("{STRING 16}'s {STRING 23}\nwhipped up a sandstorm!");
static const u8 sText_PkmnsXIntensifiedSun[] = _("{STRING 16}'s {STRING 23}\nintensified the sun's rays!");
static const u8 sText_PkmnsXPreventsYLoss[] = _("{STRING 16}'s {STRING 23}\nprevents {STRING 0} loss!");
static const u8 sText_PkmnsXInfatuatedY[] = _("{DEFENDING_MON}'s {STRING 22}\ninfatuated {ATTACKING_MON}!");
static const u8 sText_PkmnsXMadeYIneffective[] = _("{DEFENDING_MON}'s {STRING 22}\nmade {STRING 17} ineffective!");
static const u8 sText_PkmnsXCuredYProblem[] = _("{STRING 16}'s {STRING 23}\ncured its {STRING 0} problem!");
static const u8 sText_ItSuckedLiquidOoze[] = _("It sucked up the\nLIQUID OOZE!");
static const u8 sText_PkmnTransformed[] = _("{STRING 16} transformed!");
static const u8 sText_PkmnsXTookAttack[] = _("{DEFENDING_MON}'s {STRING 22}\ntook the attack!");
const u8 gText_PkmnsXPreventsSwitching[] = _("{STRING 0}'s {STRING 20}\nprevents switching!\p");
static const u8 sText_PreventedFromWorking[] = _("{DEFENDING_MON}'s {STRING 22}\nprevented {STRING 16}'s\l{STRING 0} from working!");
static const u8 sText_PkmnsXMadeItIneffective[] = _("{STRING 16}'s {STRING 23}\nmade it ineffective!");
static const u8 sText_PkmnsXPreventsFlinching[] = _("{STRING 14}'s {STRING 24}\nprevents flinching!");
static const u8 sText_PkmnsXPreventsYsZ[] = _("{ATTACKING_MON}'s {STRING 21}\nprevents {DEFENDING_MON}'s\l{STRING 22} from working!");
static const u8 sText_PkmnsXCuredItsYProblem[] = _("{STRING 16}'s {STRING 23}\ncured its {STRING 0} problem!");
static const u8 sText_PkmnsXHadNoEffectOnY[] = _("{STRING 16}'s {STRING 23}\nhad no effect on {STRING 14}!");
static const u8 sText_StatSharply[] = _("sharply ");
const u8 gText_StatRose[] = _("rose!");
static const u8 sText_StatHarshly[] = _("harshly ");
static const u8 sText_StatFell[] = _("fell!");
static const u8 sText_AttackersStatRose[] = _("{ATTACKING_MON}'s {STRING 0}\n{STRING 1}");
const u8 gText_DefendersStatRose[] = _("{DEFENDING_MON}'s {STRING 0}\n{STRING 1}");
static const u8 sText_UsingItemTheStatOfPkmnRose[] = _("Using {STRING 19}, the {STRING 0}\nof {STRING 16} {STRING 1}");
static const u8 sText_AttackersStatFell[] = _("{ATTACKING_MON}'s {STRING 0}\n{STRING 1}");
static const u8 sText_DefendersStatFell[] = _("{DEFENDING_MON}'s {STRING 0}\n{STRING 1}");
static const u8 sText_StatsWontIncrease2[] = _("{ATTACKING_MON}'s stats won't\ngo any higher!");
static const u8 sText_StatsWontDecrease2[] = _("{DEFENDING_MON}'s stats won't\ngo any lower!");
static const u8 sText_CriticalHit[] = _("A critical hit!");
static const u8 sText_OneHitKO[] = _("It's a one-hit KO!");
static const u8 sText_123Poof[] = _("{PAUSE 32}1, {PAUSE 15}2 and...{PAUSE 15} ...{PAUSE 15} ...{PAUSE 15}\n{PAUSE 15}{PLAY_SE SE_BALL_BOUNCE_1}Poof!\p");
static const u8 sText_AndEllipsis[] = _("And...\p");
static const u8 sText_HMMovesCantBeForgotten[] = _("HM moves can't be\nforgotten now.\p");
static const u8 sText_NotVeryEffective[] = _("It's not very effective...");
static const u8 sText_SuperEffective[] = _("It's super effective!");
static const u8 sText_GotAwaySafely[] = _("{PLAY_SE SE_FLEE}Got away safely!\p");
static const u8 sText_PkmnFledUsingIts[] = _("{PLAY_SE SE_FLEE}{ATTACKING_MON} fled\nusing its {STRING 19}!\p");
static const u8 sText_PkmnFledUsing[] = _("{PLAY_SE SE_FLEE}{ATTACKING_MON} fled\nusing {STRING 21}!\p");
static const u8 sText_WildPkmnFled[] = _("{PLAY_SE SE_FLEE}Wild {STRING 0} fled!");
static const u8 sText_PlayerDefeatedLinkTrainer[] = _("Player defeated\n{STRING 29}!");
static const u8 sText_TwoLinkTrainersDefeated[] = _("Player beat {STRING 30}\nand {STRING 29}!");
static const u8 sText_PlayerLostAgainstLinkTrainer[] = _("Player lost against\n{STRING 29}!");
static const u8 sText_PlayerLostToTwo[] = _("Player lost to {STRING 30}\nand {STRING 29}!");
static const u8 sText_PlayerBattledToDrawLinkTrainer[] = _("Player tied against\n{STRING 29}!");
static const u8 sText_PlayerBattledToDrawVsTwo[] = _("Player tied against {STRING 30}\nand {STRING 29}!");
static const u8 sText_WildFled[] = _("{PLAY_SE SE_FLEE}{STRING 29} fled!");
static const u8 sText_TwoWildFled[] = _("{PLAY_SE SE_FLEE}{STRING 29} and\n{STRING 30} fled!");
static const u8 sText_NoRunningFromTrainers[] = _("No! There's no running\nfrom a TRAINER battle!\p");
static const u8 sText_CantEscape[] = _("Can't escape!\p");
static const u8 sText_DontLeaveBirch[] = _("PROF. BIRCH: D-don't leave\nme here like this!\p");
static const u8 sText_ButNothingHappened[] = _("But nothing happened!");
static const u8 sText_ButItFailed[] = _("But it failed!");
static const u8 sText_ItHurtConfusion[] = _("It hurt itself in its\nconfusion!");
static const u8 sText_MirrorMoveFailed[] = _("The MIRROR MOVE failed!");
static const u8 sText_StartedToRain[] = _("It started to rain!");
static const u8 sText_DownpourStarted[] = _("A downpour started!"); // corresponds to DownpourText in pokegold and pokecrystal and is used by Rain Dance in GSC
static const u8 sText_RainContinues[] = _("Rain continues to fall.");
static const u8 sText_DownpourContinues[] = _("The downpour continues."); // unused
static const u8 sText_RainStopped[] = _("The rain stopped.");
static const u8 sText_SandstormBrewed[] = _("A sandstorm brewed!");
static const u8 sText_SandstormRages[] = _("The sandstorm rages.");
static const u8 sText_SandstormSubsided[] = _("The sandstorm subsided.");
static const u8 sText_SunlightGotBright[] = _("The sunlight got bright!");
static const u8 sText_SunlightStrong[] = _("The sunlight is strong.");
static const u8 sText_SunlightFaded[] = _("The sunlight faded.");
static const u8 sText_StartedHail[] = _("It started to hail!");
static const u8 sText_HailContinues[] = _("Hail continues to fall.");
static const u8 sText_HailStopped[] = _("The hail  stopped.");
static const u8 sText_FailedToSpitUp[] = _("But it failed to SPIT UP\na thing!");
static const u8 sText_FailedToSwallow[] = _("But it failed to SWALLOW\na thing!");
static const u8 sText_WindBecameHeatWave[] = _("The wind turned into a\nHEAT WAVE!");
static const u8 sText_StatChangesGone[] = _("All stat changes were\neliminated!");
static const u8 sText_CoinsScattered[] = _("Coins scattered everywhere!");
static const u8 sText_TooWeakForSubstitute[] = _("It was too weak to make\na SUBSTITUTE!");
static const u8 sText_SharedPain[] = _("The battlers shared\ntheir pain!");
static const u8 sText_BellChimed[] = _("A bell chimed!");
static const u8 sText_FaintInThree[] = _("All affected POKéMON will\nfaint in 3 turns!");
static const u8 sText_NoPPLeft[] = _("There's no PP left for\nthis move!\p");
static const u8 sText_ButNoPPLeft[] = _("But there was no PP left\nfor the move!");
static const u8 sText_PkmnIgnoresAsleep[] = _("{ATTACKING_MON} ignored\norders while asleep!");
static const u8 sText_PkmnIgnoredOrders[] = _("{ATTACKING_MON} ignored\norders!");
static const u8 sText_PkmnBeganToNap[] = _("{ATTACKING_MON} began to nap!");
static const u8 sText_PkmnLoafing[] = _("{ATTACKING_MON} is\nloafing around!");
static const u8 sText_PkmnWontObey[] = _("{ATTACKING_MON} won't\nobey!");
static const u8 sText_PkmnTurnedAway[] = _("{ATTACKING_MON} turned away!");
static const u8 sText_PkmnPretendNotNotice[] = _("{ATTACKING_MON} pretended\nto not notice!");
static const u8 sText_EnemyAboutToSwitchPkmn[] = _("{STRING 25} {STRING 26} is\nabout to use {STRING 1}.\pWill {STRING 32} change\nPOKéMON?");
static const u8 sText_PkmnLearnedMove2[] = _("{ATTACKING_MON} learned\n{STRING 0}!");
static const u8 sText_PlayerDefeatedLinkTrainerTrainer1[] = _("Player defeated\n{STRING 25} {STRING 26}!\p");
static const u8 sText_CreptCloser[] = _("{STRING 32} crept closer to\n{STRING 3}!");
static const u8 sText_CantGetCloser[] = _("{STRING 32} can't get any closer!");
static const u8 sText_PkmnWatchingCarefully[] = _("{STRING 3} is watching\ncarefully!");
static const u8 sText_PkmnCuriousAboutX[] = _("{STRING 3} is curious about\nthe {STRING 0}!");
static const u8 sText_PkmnEnthralledByX[] = _("{STRING 3} is enthralled by\nthe {STRING 0}!");
static const u8 sText_PkmnIgnoredX[] = _("{STRING 3} completely ignored\nthe {STRING 0}!");
static const u8 sText_ThrewPokeblockAtPkmn[] = _("{STRING 32} threw a {POKEBLOCK}\nat the {STRING 3}!");
static const u8 sText_OutOfSafariBalls[] = _("{PLAY_SE SE_DING_DONG}ANNOUNCER: You're out of\nSAFARI BALLS! Game over!\p");
static const u8 sText_WildPkmnAppeared[] = _("Wild {STRING 3} appeared!\p");
static const u8 sText_LegendaryPkmnAppeared[] = _("Wild {STRING 3} appeared!\p");
static const u8 sText_WildPkmnAppearedPause[] = _("Wild {STRING 3} appeared!{PAUSE 127}");
static const u8 sText_TwoWildPkmnAppeared[] = _("Wild {STRING 5} and\n{STRING 3} appeared!\p");
static const u8 sText_Trainer1WantsToBattle[] = _("{STRING 25} {STRING 26}\nwould like to battle!\p");
static const u8 sText_LinkTrainerWantsToBattle[] = _("{STRING 29}\nwants to battle!");
static const u8 sText_TwoLinkTrainersWantToBattle[] = _("{STRING 29} and {STRING 30}\nwant to battle!");
static const u8 sText_Trainer1SentOutPkmn[] = _("{STRING 25} {STRING 26} sent\nout {STRING 3}!");
static const u8 sText_Trainer1SentOutTwoPkmn[] = _("{STRING 25} {STRING 26} sent\nout {STRING 3} and {STRING 5}!");
static const u8 sText_Trainer1SentOutPkmn2[] = _("{STRING 25} {STRING 26} sent\nout {STRING 0}!");
static const u8 sText_LinkTrainerSentOutPkmn[] = _("{STRING 29} sent\nout {STRING 3}!");
static const u8 sText_LinkTrainerSentOutTwoPkmn[] = _("{STRING 29} sent out\n{STRING 3} and {STRING 5}!");
static const u8 sText_TwoLinkTrainersSentOutPkmn[] = _("{STRING 29} sent out\n{STRING 7}!\p{STRING 30} sent out\n{STRING 9}!");
static const u8 sText_LinkTrainerSentOutPkmn2[] = _("{STRING 29} sent out\n{STRING 0}!");
static const u8 sText_LinkTrainerMultiSentOutPkmn[] = _("{STRING 31} sent out\n{STRING 0}!");
static const u8 sText_GoPkmn[] = _("Go! {STRING 2}!");
static const u8 sText_GoTwoPkmn[] = _("Go! {STRING 2} and\n{STRING 4}!");
static const u8 sText_GoPkmn2[] = _("Go! {STRING 0}!");
static const u8 sText_DoItPkmn[] = _("Do it! {STRING 0}!");
static const u8 sText_GoForItPkmn[] = _("Go for it, {STRING 0}!");
static const u8 sText_YourFoesWeakGetEmPkmn[] = _("Your foe's weak!\nGet'm, {STRING 0}!");
static const u8 sText_LinkPartnerSentOutPkmnGoPkmn[] = _("{STRING 28} sent out\n{STRING 8}!\pGo! {STRING 6}!");
static const u8 sText_PkmnThatsEnough[] = _("{STRING 0}, that's enough!\nCome back!");
static const u8 sText_PkmnComeBack[] = _("{STRING 0}, come back!");
static const u8 sText_PkmnOkComeBack[] = _("{STRING 0}, OK!\nCome back!");
static const u8 sText_PkmnGoodComeBack[] = _("{STRING 0}, good!\nCome back!");
static const u8 sText_Trainer1WithdrewPkmn[] = _("{STRING 25} {STRING 26}\nwithdrew {STRING 0}!");
static const u8 sText_LinkTrainer1WithdrewPkmn[] = _("{STRING 29} withdrew\n{STRING 0}!");
static const u8 sText_LinkTrainer2WithdrewPkmn[] = _("{STRING 31} withdrew\n{STRING 0}!");
static const u8 sText_WildPkmnPrefix[] = _("Wild ");
static const u8 sText_FoePkmnPrefix[] = _("Foe ");
static const u8 sText_FoePkmnPrefix2[] = _("Foe ");
static const u8 sText_AllyPkmnPrefix[] = _("Ally ");
static const u8 sText_FoePkmnPrefix3[] = _("Foe ");
static const u8 sText_AllyPkmnPrefix2[] = _("Ally ");
static const u8 sText_FoePkmnPrefix4[] = _("Foe ");
static const u8 sText_AllyPkmnPrefix3[] = _("Ally ");
static const u8 sText_AttackerUsedX[] = _("{ATTACKING_MON} used\n{STRING 1}");
static const u8 sText_ExclamationMark[] = _("!");
static const u8 sText_ExclamationMark2[] = _("!");
static const u8 sText_ExclamationMark3[] = _("!");
static const u8 sText_ExclamationMark4[] = _("!");
static const u8 sText_ExclamationMark5[] = _("!");
static const u8 sText_HP2[] = _("HP");
static const u8 sText_Attack2[] = _("ATTACK");
static const u8 sText_Defense2[] = _("DEFENSE");
static const u8 sText_Speed[] = _("SPEED");
static const u8 sText_SpAtk2[] = _("SP. ATK");
static const u8 sText_SpDef2[] = _("SP. DEF");
static const u8 sText_Accuracy[] = _("accuracy");
static const u8 sText_Evasiveness[] = _("evasiveness");
static const u8 sText_PokeblockWasTooSpicy[] = _("was too spicy!");
static const u8 sText_PokeblockWasTooDry[] = _("was too dry!");
static const u8 sText_PokeblockWasTooSweet[] = _("was too sweet!");
static const u8 sText_PokeblockWasTooBitter[] = _("was too bitter!");
static const u8 sText_PokeblockWasTooSour[] = _("was too sour!");
static const u8 sText_PlayerUsedItem[] = _("{STRING 32} used\n{STRING 19}!");
static const u8 sText_WallyUsedItem[] = _("WALLY used\n{STRING 19}!");
static const u8 sText_Trainer1UsedItem[] = _("{STRING 25} {STRING 26}\nused {STRING 19}!");
static const u8 sText_TrainerBlockedBall[] = _("The TRAINER blocked the BALL!");
static const u8 sText_DontBeAThief[] = _("Don't be a thief!");
static const u8 sText_ItDodgedBall[] = _("It dodged the thrown BALL!\nThis POKéMON can't be caught!");
static const u8 sText_YouMissedPkmn[] = _("You missed the POKéMON!");
static const u8 sText_PkmnBrokeFree[] = _("Oh, no!\nThe POKéMON broke free!");
static const u8 sText_ItAppearedCaught[] = _("Aww!\nIt appeared to be caught!");
static const u8 sText_AarghAlmostHadIt[] = _("Aargh!\nAlmost had it!");
static const u8 sText_ShootSoClose[] = _("Shoot!\nIt was so close, too!");
static const u8 sText_GotchaPkmnCaughtPlayer[] = _("Gotcha!\n{STRING 3} was caught!{UNKNOWN_A}{PLAY_BGM MUS_CAUGHT}\p");
static const u8 sText_GotchaPkmnCaughtWally[] = _("Gotcha!\n{STRING 3} was caught!{UNKNOWN_A}{PLAY_BGM MUS_CAUGHT}{PAUSE 127}");
static const u8 sText_GiveNicknameCaptured[] = _("Give a nickname to the\ncaptured {STRING 3}?");
static const u8 sText_PkmnSentToPC[] = _("{STRING 3} was sent to\n{STRING 35} PC.");
static const u8 sText_Someones[] = _("someone's");
static const u8 sText_Lanettes[] = _("LANETTE's");
static const u8 sText_PkmnDataAddedToDex[] = _("{STRING 3}'s data was\nadded to the POKéDEX.\p");
static const u8 sText_ItIsRaining[] = _("It is raining.");
static const u8 sText_SandstormIsRaging[] = _("A sandstorm is raging.");
static const u8 sText_BoxIsFull[] = _("The BOX is full!\nYou can't catch any more!\p");
static const u8 sText_EnigmaBerry[] = _("ENIGMA BERRY");
static const u8 sText_BerrySuffix[] = _(" BERRY");
static const u8 sText_PkmnsItemCuredParalysis[] = _("{STRING 16}'s {STRING 19}\ncured paralysis!");
static const u8 sText_PkmnsItemCuredPoison[] = _("{STRING 16}'s {STRING 19}\ncured poison!");
static const u8 sText_PkmnsItemHealedBurn[] = _("{STRING 16}'s {STRING 19}\nhealed its burn!");
static const u8 sText_PkmnsItemDefrostedIt[] = _("{STRING 16}'s {STRING 19}\ndefrosted it!");
static const u8 sText_PkmnsItemWokeIt[] = _("{STRING 16}'s {STRING 19}\nwoke it from its sleep!");
static const u8 sText_PkmnsItemSnappedOut[] = _("{STRING 16}'s {STRING 19}\nsnapped it out of confusion!");
static const u8 sText_PkmnsItemCuredProblem[] = _("{STRING 16}'s {STRING 19}\ncured its {STRING 0} problem!");
static const u8 sText_PkmnsItemNormalizedStatus[] = _("{STRING 16}'s {STRING 19}\nnormalized its status!");
static const u8 sText_PkmnsItemRestoredHealth[] = _("{STRING 16}'s {STRING 19}\nrestored health!");
static const u8 sText_PkmnsItemRestoredPP[] = _("{STRING 16}'s {STRING 19}\nrestored {STRING 0}'s PP!");
static const u8 sText_PkmnsItemRestoredStatus[] = _("{STRING 16}'s {STRING 19}\nrestored its status!");
static const u8 sText_PkmnsItemRestoredHPALittle[] = _("{STRING 16}'s {STRING 19}\nrestored its HP a little!");
static const u8 sText_ItemAllowsOnlyYMove[] = _("{STRING 19} allows the\nuse of only {STRING 17}!\p");
static const u8 sText_PkmnHungOnWithX[] = _("{DEFENDING_MON} hung on\nusing its {STRING 19}!");
const u8 gText_EmptyString3[] = _("");
static const u8 sText_YouThrowABallNowRight[] = _("You throw a BALL now, right?\nI... I'll do my best!");
const u8 gText_PkmnIsEvolving[] = _("What?\n{STRING 2} is evolving!");
const u8 gText_CongratsPkmnEvolved[] = _("Congratulations! Your {STRING 2}\nevolved into {STRING 3}!{UNKNOWN_A}\p");
const u8 gText_PkmnStoppedEvolving[] = _("Huh? {STRING 2}\nstopped evolving!\p");
const u8 gText_WhatWillPkmnDo[] = _("What should\n{STRING 15} do?");
const u8 gText_WhatWillPkmnDo2[] = _("What will\n{STRING 32} do?");
const u8 gText_WhatWillWallyDo[] = _("What will\nWALLY do?");
const u8 gText_LinkStandby[] = _("{PAUSE 16}Link standby...");
const u8 gText_BattleMenu[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}FIGHT{CLEAR_TO 0x2E}BAG\nPOKéMON{CLEAR_TO 0x2E}RUN");
const u8 gText_SafariZoneMenu[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}BALL{CLEAR_TO 0x2E}{POKEBLOCK}\nGO NEAR{CLEAR_TO 0x2E}RUN");
const u8 gText_MoveInterfacePPType[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}PP\nTYPE/");
const u8 gText_MoveInterfaceDynamicColors[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}"); // cant think of a better name
const u8 gText_WhichMoveToForget4[] = _("{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}{PALETTE 5}Which move should\nbe forgotten?");
const u8 gText_BattleYesNoChoice[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}Yes\nNo");
const u8 gText_BattleSwitchWhich[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}Switch\nwhich?");
const u8 gText_BattleSwitchWhich2[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW SKY_BLUE LIGHT_BLUE WHITE2}");
const u8 BattleText_RightArrow[] = _("{RIGHT_ARROW}");
const u8 BattleText_Plus[] = _("+");
const u8 BattleText_Dash[] = _("-");
static const u8 sText_HP[] = _("HP     ");
static const u8 sText_Attack[] = _("ATTACK ");
static const u8 sText_Defense[] = _("DEFENSE");
static const u8 sText_SpAtk[] = _("SP. ATK");
static const u8 sText_SpDef[] = _("SP. DEF");
const u8 gText_SafariBalls[] = _("{HIGHLIGHT RED}SAFARI BALLS");
const u8 gText_SafariBallLeft[] = _("{HIGHLIGHT RED}Left: ");
const u8 BattleText_HighlightRed[] = _("{HIGHLIGHT RED}");
const u8 gText_Sleep[] = _("sleep");
const u8 gText_Poison[] = _("poison");
const u8 gText_Burn[] = _("burn");
const u8 gText_Paralysis[] = _("paralysis");
const u8 gText_Ice[] = _("ice");
const u8 gText_Confusion[] = _("confusion");
const u8 gText_Love[] = _("love");
const u8 gText_SpaceAndSpace[] = _(" and ");
const u8 gText_CommaSpace[] = _(", ");
const u8 gText_Space2[] = _(" ");
const u8 gText_LineBreak[] = _("\l");
const u8 gText_NewLine[] = _("\n");
const u8 BattleText_Format8[] = _("are");
const u8 BattleText_Format9[] = _("are");
const u8 gBadEggNickname[] = _("Bad EGG"); // 8400E3A
const u8 gText_BattleWallyName[] = _("WALLY");
const u8 gText_Win[] = _("{HIGHLIGHT TRANSPARENT}Win"); // 8400E48
const u8 gText_Loss[] = _("{HIGHLIGHT TRANSPARENT}Loss"); // 8400E4F
const u8 gText_Draw[] = _("{HIGHLIGHT TRANSPARENT}Tie"); // 8400E57
static const u8 sText_SpaceIs[] = _(" is");
static const u8 sText_ApostropheS[] = _("'s");
const u8 BattleText_Normal[] = _("a NORMAL move");
const u8 BattleText_Fighting[] = _("a FIGHTING move");
const u8 BattleText_Flying[] = _("a FLYING move");
const u8 BattleText_Poison[] = _("a POISON move");
const u8 BattleText_Ground[] = _("a GROUND move");
const u8 BattleText_Rock[] = _("a ROCK move");
const u8 BattleText_Bug[] = _("a BUG move");
const u8 BattleText_Ghost[] = _("a GHOST move");
const u8 BattleText_Steel[] = _("a STEEL move");
const u8 BattleText_Typeless[] = _("a ??? move");
const u8 BattleText_Fire[] = _("a FIRE move");
const u8 BattleText_Water[] = _("a WATER move");
const u8 BattleText_Grass[] = _("a GRASS move");
const u8 BattleText_Electric[] = _("an ELECTRIC move");
const u8 BattleText_Psychic[] = _("a PSYCHIC move");
const u8 BattleText_Ice[] = _("an ICE move");
const u8 BattleText_Dragon[] = _("a DRAGON move");
const u8 BattleText_Dark[] = _("a DARK move");

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
    BATTLE_TEXT_AttackMissed,
    BATTLE_TEXT_ProtectedItself,
    BATTLE_TEXT_AvoidedAttack,
    BATTLE_TEXT_AvoidedDamage,
    BATTLE_TEXT_GroundMoveNegate,
};

const u16 gNoEscapeStringIds[] =
{
    BATTLE_TEXT_CantEscape2,
    BATTLE_TEXT_BirchDontLeaveMe,
    BATTLE_TEXT_PreventEscape,
    BATTLE_TEXT_CantEscape,
    BATTLE_TEXT_AttackingCantEscape,
};

const u16 gMoveWeatherChangeStringIds[] =
{
    BATTLE_TEXT_RainStart,
    BATTLE_TEXT_PourStart,
    BATTLE_TEXT_Failed,
    BATTLE_TEXT_SandBrewed,
    BATTLE_TEXT_SunBright,
    BATTLE_TEXT_HailStart,
};

const u16 gSandStormHailContinuesStringIds[] =
{
    BATTLE_TEXT_SandRages,
    BATTLE_TEXT_HailCont,
};

const u16 gSandStormHailDmgStringIds[] =
{
    BATTLE_TEXT_SandBuffeted,
    BATTLE_TEXT_HailStricken,
};

const u16 gSandStormHailEndStringIds[] =
{
    BATTLE_TEXT_SandSubsided,
    BATTLE_TEXT_HailStop,
};

const u16 gRainContinuesStringIds[] =
{
    BATTLE_TEXT_RainCont1,
    BATTLE_TEXT_PourCont,
    BATTLE_TEXT_RainStop,
};

const u16 gProtectLikeUsedStringIds[] =
{
    BATTLE_TEXT_ProtectedItself2,
    BATTLE_TEXT_BracedItself,
    BATTLE_TEXT_Failed,
};

const u16 gReflectLightScreenSafeguardStringIds[] =
{
    BATTLE_TEXT_Failed,
    BATTLE_TEXT_RaisedDefense,
    BATTLE_TEXT_RaisedDefenseLittle,
    BATTLE_TEXT_RaisedSpDef,
    BATTLE_TEXT_RaisedSpDefLittle,
    BATTLE_TEXT_CoveredVeil,
};

const u16 gLeechSeedStringIds[] =
{
    BATTLE_TEXT_WasSeeded,
    BATTLE_TEXT_EvadedAttack,
    BATTLE_TEXT_DoesntAffect,
    BATTLE_TEXT_HealthSapped,
    BATTLE_TEXT_OozeSuckup,
};

const u16 gRestUsedStringIds[] =
{
    BATTLE_TEXT_WentToSleep,
    BATTLE_TEXT_SpeltHealthy,
};

const u16 gUproarOverTurnStringIds[] =
{
    BATTLE_TEXT_UproarMaking,
    BATTLE_TEXT_CalmedDown,
};

const u16 gStockpileUsedStringIds[] =
{
    BATTLE_TEXT_Stockpiled,
    BATTLE_TEXT_StockpiledCant,
};

const u16 gWokeUpStringIds[] =
{
    BATTLE_TEXT_WokeUp,
    BATTLE_TEXT_UproarWoke,
};

const u16 gSwallowFailStringIds[] =
{
    BATTLE_TEXT_SwallowFail,
    BATTLE_TEXT_HPFull,
};

const u16 gUproarAwakeStringIds[] =
{
    BATTLE_TEXT_UproarCantSleep2,
    BATTLE_TEXT_UproarKeptAwake,
    BATTLE_TEXT_StayedAwake,
};

const u16 gStatUpStringIds[] =
{
    BATTLE_TEXT_UnknownString7,
    BATTLE_TEXT_UnknownString3,
    BATTLE_TEXT_AttackingStatNoHigher,
    BATTLE_TEXT_Terminator2,
    BATTLE_TEXT_UnknownString4,
    BATTLE_TEXT_HustleUse,
};

const u16 gStatDownStringIds[] =
{
    BATTLE_TEXT_UnknownString5,
    BATTLE_TEXT_UnknownString6,
    BATTLE_TEXT_DefendingStatNoHigher,
    BATTLE_TEXT_Terminator2,
};

const u16 gFirstTurnOfTwoStringIds[] =
{
    BATTLE_TEXT_WhipWhirlwind,
    BATTLE_TEXT_TookSunlight,
    BATTLE_TEXT_LoweredHead,
    BATTLE_TEXT_IsGlowing,
    BATTLE_TEXT_FlewHigh,
    BATTLE_TEXT_DugHole,
    BATTLE_TEXT_HidUnderwater,
    BATTLE_TEXT_SprangUp,
};

const u16 gWrappedStringIds[] =
{
    BATTLE_TEXT_SqueezedBind, 
    BATTLE_TEXT_Wrapped, 
    BATTLE_TEXT_TrappedVortex, 
    BATTLE_TEXT_Clamped, 
    BATTLE_TEXT_TrappedVortex, 
    BATTLE_TEXT_SandTombTrapped, 
};

const u16 gMistUsedStringIds[] =
{
    BATTLE_TEXT_MistShroud,
    BATTLE_TEXT_Failed,
};

const u16 gFocusEnergyUsedStringIds[] =
{
    BATTLE_TEXT_GetPumped,
    BATTLE_TEXT_Failed,
};

const u16 gTransformUsedStringIds[] =
{
    BATTLE_TEXT_Transformed,
    BATTLE_TEXT_Failed,
};

const u16 gSubsituteUsedStringIds[] =
{
    BATTLE_TEXT_SubMade,
    BATTLE_TEXT_SubWeak,
};

const u16 gGotPoisonedStringIds[] =
{
    BATTLE_TEXT_Poisoned,
    BATTLE_TEXT_Poisoned2,
};

const u16 gGotParalyzedStringIds[] =
{
    BATTLE_TEXT_Paralyzed,
    BATTLE_TEXT_Paralyzed2,
};

const u16 gFellAsleepStringIds[] =
{
    BATTLE_TEXT_FellAsleep,
    BATTLE_TEXT_MadeAsleep,
};

const u16 gGotBurnedStringIds[] =
{
    BATTLE_TEXT_Burned,
    BATTLE_TEXT_Burned2,
};

const u16 gGotFrozenStringIds[] =
{
    BATTLE_TEXT_Frozen,
    BATTLE_TEXT_Frozen2,
};

const u16 gGotDefrostedStringIds[] =
{
    BATTLE_TEXT_AttackingDefrosted,
    BATTLE_TEXT_Defrosted,
};

const u16 gKOFailedStringIds[] =
{
    BATTLE_TEXT_AttackMissed,
    BATTLE_TEXT_Unaffected,
    BATTLE_TEXT_FellLove,
    BATTLE_TEXT_InfatuatedPoke,
};

const u16 gLeechSeedDrainStringIds[] =
{
    BATTLE_TEXT_EnergyDrained,
    BATTLE_TEXT_OozeSuckup,
};

const u16 gSportsUsedStringIds[] =
{
    BATTLE_TEXT_ElecWeakened,
    BATTLE_TEXT_FireWeakened,
};

const u16 gPartyStatusHealStringIds[] =
{
    BATTLE_TEXT_BellChime,
    BATTLE_TEXT_BellChime,
    BATTLE_TEXT_BellChime,
    BATTLE_TEXT_BellChime,
    BATTLE_TEXT_SoothingAroma,
};

const u16 gFutureMoveUsedStringIds[] =
{
    BATTLE_TEXT_ForesawAttack,
    BATTLE_TEXT_ChoseDestiny,
};

const u16 gBallEscapeStringIds[] =
{
    BATTLE_TEXT_BallBrokeOhNo,
    BATTLE_TEXT_BallBrokeAppeared,
    BATTLE_TEXT_BallBrokeAlmost,
    BATTLE_TEXT_BallBrokeSoClose,
};

const u16 gWeatherContinuesStringIds[] =
{
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Sandstorm,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_SunStrong,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
    BATTLE_TEXT_Raining,
};

const u16 gInobedientStringIds[] =
{
    BATTLE_TEXT_LoafingAround, 
    BATTLE_TEXT_WontObey, 
    BATTLE_TEXT_TurnedAway, 
    BATTLE_TEXT_NotNotice, 
};

const u16 gSafariGetNearStringIds[] =
{
    BATTLE_TEXT_CreptCloser,
    BATTLE_TEXT_CantGetCloser,
};

const u16 gSafariPokeblockResultStringIds[] =
{
    BATTLE_TEXT_CuriousAbout,
    BATTLE_TEXT_EnthralledBy,
    BATTLE_TEXT_IgnoredThing,
};

const u16 gTrainerItemCuredStatusStringIds[] =
{
    BATTLE_TEXT_CuredConfusion,
    BATTLE_TEXT_CuredParalysis,
    BATTLE_TEXT_CuredFreeze,
    BATTLE_TEXT_CuredBurn,
    BATTLE_TEXT_CuredPoison,
    BATTLE_TEXT_CuredSleep,
};

const u16 gBerryEffectStringIds[] =
{
    BATTLE_TEXT_CuredStatus,
    BATTLE_TEXT_NormalizedStatus,
};

const u16 gBRNPreventionStringIds[] =
{
    BATTLE_TEXT_PreventedBurn,
    BATTLE_TEXT_PreventedOther2,
    BATTLE_TEXT_NoEffectOn,
};

const u16 gPRLZPreventionStringIds[] =
{
    BATTLE_TEXT_PreventedPara,
    BATTLE_TEXT_PreventedOther2,
    BATTLE_TEXT_NoEffectOn,
};

const u16 gPSNPreventionStringIds[] =
{
    BATTLE_TEXT_PreventedPoison,
    BATTLE_TEXT_PreventedOther2,
    BATTLE_TEXT_NoEffectOn,
};

const u16 gItemSwapStringIds[] =
{
    BATTLE_TEXT_Obtained1,
    BATTLE_TEXT_Obtained2,
    BATTLE_TEXT_Obtained3,
};

const u16 gFlashFireStringIds[] =
{
    BATTLE_TEXT_RaisedFirePower,
    BATTLE_TEXT_MadeIneffective,
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
