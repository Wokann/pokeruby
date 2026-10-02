EventScript_StartTrainerBattle:: @ 819F80B
	lock
	special PlayTrainerEncounterMusic
	special DoTrainerApproach
	goto EventScript_DoTrainerBattle

EventScript_TryDoNormalTrainerBattle:: @ 819F818
	lock
	faceplayer
	applymovement VAR_LAST_TALKED, Movement_RevealTrainer
	waitmovement 0
	specialvar VAR_RESULT, GetTrainerFlag
	goto_if_ne VAR_RESULT, FALSE, EventScript_NoNormalTrainerBattle
	special PlayTrainerEncounterMusic
	special SetTrainerFacingDirection
	goto EventScript_DoTrainerBattle

EventScript_NoNormalTrainerBattle:
	gotopostbattlescript

EventScript_TryDoDoubleTrainerBattle:: @ 819F840
	lock
	faceplayer
	call EventScript_RevealTrainer
	specialvar VAR_RESULT, GetTrainerFlag
	goto_if_ne VAR_RESULT, FALSE, EventScript_NoDoubleTrainerBattle
	special HasEnoughMonsForDoubleBattle
	goto_if_ne VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, EventScript_NotEnoughMonsForDoubleBattle
	special PlayTrainerEncounterMusic
	special SetTrainerFacingDirection
	goto EventScript_DoTrainerBattle

EventScript_NotEnoughMonsForDoubleBattle:
	special ShowTrainerCantBattleSpeech
	waitmessage
	waitbuttonpress
	release
	end

EventScript_NoDoubleTrainerBattle:
	gotopostbattlescript

EventScript_DoNoIntroTrainerBattle:: @ 819F878
	applymovement VAR_LAST_TALKED, Movement_RevealTrainer
	waitmovement 0
	special PlayTrainerEncounterMusic
	dotrainerbattle
	gotopostbattlescript

EventScript_TryDoRematchBattle:: @ 819F887
	call EventScript_RevealTrainer
	specialvar VAR_RESULT, IsTrainerReadyForRematch
	goto_if_eq VAR_RESULT, FALSE, EventScript_NoRematchTrainerBattle
	special PlayTrainerEncounterMusic
	special SetTrainerFacingDirection
	special ShowTrainerIntroSpeech
	waitmessage
	waitbuttonpress
	special BattleSetup_StartRematchBattle
	releaseall
	end

EventScript_NoRematchTrainerBattle:
	gotopostbattlescript

EventScript_TryDoDoubleRematchBattle:: @ 819F8AE
	specialvar VAR_RESULT, IsTrainerReadyForRematch
	goto_if_eq VAR_RESULT, FALSE, EventScript_NoDoubleRematchTrainerBattle
	special HasEnoughMonsForDoubleBattle
	goto_if_ne VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, EventScript_NotEnoughMonsForDoubleRematchBattle
	special PlayTrainerEncounterMusic
	special SetTrainerFacingDirection
	special ShowTrainerIntroSpeech
	waitmessage
	waitbuttonpress
	special BattleSetup_StartRematchBattle
	releaseall
	end

EventScript_NoDoubleRematchTrainerBattle:
	gotopostbattlescript

EventScript_NotEnoughMonsForDoubleRematchBattle:
	special ShowTrainerCantBattleSpeech
	waitmessage
	waitbuttonpress
	release
	end

EventScript_RevealTrainer:
	applymovement VAR_LAST_TALKED, Movement_RevealTrainer
	waitmovement 0
	return

Movement_RevealTrainer::
	reveal_trainer
	step_end

EventScript_DoTrainerBattle:
	special ShowTrainerIntroSpeech
	waitmessage
	waitbuttonpress
	dotrainerbattle
	specialvar VAR_RESULT, GetTrainerBattleMode
	goto_if_eq VAR_RESULT, TRAINER_BATTLE_SINGLE, EventScript_EndTrainerBattle
	goto_if_eq VAR_RESULT, TRAINER_BATTLE_CONTINUE_SCRIPT, EventScript_GoToBeatenScript
	goto_if_eq VAR_RESULT, TRAINER_BATTLE_CONTINUE_SCRIPT_NO_MUSIC, EventScript_GoToBeatenScript
	goto_if_eq VAR_RESULT, TRAINER_BATTLE_CONTINUE_SCRIPT_DOUBLE, EventScript_GoToBeatenScript
	goto_if_eq VAR_RESULT, TRAINER_BATTLE_CONTINUE_SCRIPT_DOUBLE_NO_MUSIC, EventScript_GoToBeatenScript
EventScript_EndTrainerBattle:
	releaseall
	end

EventScript_GoToBeatenScript:
	gotobeatenscript

Std_MsgboxAutoclose::
	message NULL
	waitmessage
	waitbuttonpress
	release
	return
