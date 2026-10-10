gUnknown_081A14B8:: @ 81A14B8
	lockall
	special TryFieldPoisonWhiteOut
	waitstate
	compare VAR_RESULT, 1
	goto_if_eq EventScript_1A14CA
	releaseall
	end

EventScript_1A14CA::
	message gText_PlayerWhitedOut
	waitmessage
	waitbuttonpress
	special Script_FadeOutMapMusic
	waitstate
	fadescreen FADE_TO_BLACK
	special sp0C8_whiteout_maybe
	waitstate
	end
