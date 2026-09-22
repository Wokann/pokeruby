	.include "include/macros.inc"
	.include "include/macros/contest_ai_script.inc"
	.include "constants/constants.inc"

	.section script_data, "aw", %progbits

	enum_start
	enum MON_1
	enum MON_2
	enum MON_3
	enum MON_4

	.align 2
gContestAIs:: @ 81DC118
	.4byte AI_CheckForBadMove
	.4byte AI_CheckCombo
	.4byte AI_CheckBoring
	.4byte AI_CheckExcitement
	.4byte AI_CheckOrder
	.4byte AI_CheckForGoodMove
	.4byte AI_Erratic
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing
	.4byte AI_Nothing

@ Unreferenced AI routine to encourage moves that improve condition on the first
@ turn. Additionally, it checks the appeal order of the user and the effect
@ type, but the code is buggy and doesn't affect the score.
AI_CheckTiming:
	if_appeal_num_not_eq 0, AI_CheckTiming_SkipCondition
	if_effect_not_eq CONTEST_EFFECT_IMPROVE_CONDITION_PREVENT_NERVOUSNESS, AI_CheckTiming_SkipCondition
	score +10
AI_CheckTiming_SkipCondition:
	call AI_CheckTiming_TryStartle
	end
AI_CheckTiming_TryStartle:
	if_user_order_more_than MON_2, AI_CheckTiming_End
	if_effect_type_not_eq CONTEST_EFFECT_TYPE_STARTLE_MON, AI_CheckTiming_End
	if_effect_type_not_eq CONTEST_EFFECT_TYPE_STARTLE_MONS, AI_CheckTiming_End
	score +10 @ unreachable
AI_CheckTiming_End:
	end

@ Unreferenced AI routine that doesn't make much sense.
AI_AvoidStartle:
	if_appeal_num_eq 0, AI_AvoidStartle_1stAppeal
	if_appeal_num_eq 1, AI_AvoidStartle_2ndAppeal
	if_appeal_num_eq 2, AI_AvoidStartle_3rdAppeal
	if_appeal_num_eq 3, AI_AvoidStartle_4thAppeal
	if_last_appeal AI_AvoidStartle_LastAppeal
	end
AI_AvoidStartle_1stAppeal:
	if_user_order_not_eq MON_1, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_2, AI_AvoidStartle_EncourageIfAvoidMove2
	if_user_order_not_eq MON_3, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_4, AI_AvoidStartle_EncourageIfAvoidMove
	end
AI_AvoidStartle_EncourageIfAvoidMove:
	if_effect_type_eq CONTEST_EFFECT_TYPE_AVOID_STARTLE, AI_AvoidStartle_Encourage
	end
AI_AvoidStartle_EncourageIfAvoidMove2:
	if_effect_type_eq CONTEST_EFFECT_TYPE_AVOID_STARTLE, AI_AvoidStartle_Encourage
	end
AI_AvoidStartle_EncourageIfAvoidMove3:
	if_effect_type_eq CONTEST_EFFECT_TYPE_AVOID_STARTLE, AI_AvoidStartle_Encourage
	end
AI_AvoidStartle_2ndAppeal:
	if_user_order_not_eq MON_1, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_2, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_3, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_4, AI_AvoidStartle_EncourageIfAvoidMove
	end
AI_AvoidStartle_3rdAppeal:
	if_user_order_not_eq MON_1, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_2, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_3, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_4, AI_AvoidStartle_EncourageIfAvoidMove
	end
AI_AvoidStartle_4thAppeal:
	if_user_order_not_eq MON_1, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_2, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_3, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_4, AI_AvoidStartle_EncourageIfAvoidMove
	end
AI_AvoidStartle_LastAppeal:
	if_user_order_not_eq MON_1, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_2, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_3, AI_AvoidStartle_EncourageIfAvoidMove
	if_user_order_not_eq MON_4, AI_AvoidStartle_EncourageIfAvoidMove
	end
AI_AvoidStartle_Encourage:
	score +10
	end

AI_AvoidStartle_End:
	end

@ Unreferenced AI routine to encourage the most appealing move.
AI_PreferMostAppealingMove:
	if_most_appealing_move AI_PreferMostAppealingMove_Encourage
	end
AI_PreferMostAppealingMove_Encourage:
	score +10
	end

AI_CheckBoring:
	if_effect_eq CONTEST_EFFECT_REPETITION_NOT_BORING, AI_CheckBoring_NotBoring
	if_move_used_count_eq 1, AI_CheckBoring_1stRepeat
	if_move_used_count_eq 2, AI_CheckBoring_2ndRepeat
	if_move_used_count_eq 3, AI_CheckBoring_3rdRepeat
	if_move_used_count_eq 4, AI_CheckBoring_4thRepeat
	end
AI_CheckBoring_1stRepeat:
	score -5
	end
AI_CheckBoring_2ndRepeat:
	score -15
	end
AI_CheckBoring_3rdRepeat:
	score -20
	end
AI_CheckBoring_4thRepeat:
	score -25
	end
AI_CheckBoring_NotBoring:
	end

AI_CheckExcitement:
	if_move_excitement_less_than 0, AI_CheckExcitement_Negative
	if_move_excitement_eq 0, AI_CheckExcitement_Neutral
	if_move_excitement_eq 1, AI_CheckExcitement_Positive
	end
AI_CheckExcitement_Negative:
	if_excitement_eq 4, AI_CheckExcitement_Negative_1AwayFromMax
	if_excitement_eq 3, AI_CheckExcitement_Negative_2AwayFromMax
	if_user_has_exciting_move AI_CheckExcitement_End
	score +15
	end
AI_CheckExcitement_Negative_1AwayFromMax:
	if_user_order_not_eq MON_1, AI_CheckExcitement_Negative_1AwayFromMax_Not1stUp
	if_random 51, AI_CheckExcitement_End
	score +20
	end
AI_CheckExcitement_Negative_1AwayFromMax_Not1stUp:
	if_random 127, AI_CheckExcitement_End
	score -10
	end
AI_CheckExcitement_Negative_2AwayFromMax:
	if_user_order_not_eq MON_1, AI_CheckExcitement_Negative_2AwayFromMax_Not1stUp
	if_last_appeal AI_CheckExcitement_Negative_2AwayFromMax_LastAppeal
	if_random 51, AI_CheckExcitement_End
	score +10
	end
AI_CheckExcitement_Negative_2AwayFromMax_LastAppeal:
	score +15
	end
AI_CheckExcitement_Negative_2AwayFromMax_Not1stUp:
	if_random 127, AI_CheckExcitement_End
	score +10
	end
AI_CheckExcitement_Neutral:
	if_random 127, AI_CheckExcitement_End
	score +10
	end
AI_CheckExcitement_Positive:
	if_move_used_count_more_than 0, AI_CheckExcitement_Positive_Repeat
	if_user_order_not_eq MON_1, AI_CheckExcitement_Positive_Not1stUpForMax
	if_excitement_not_eq 4, AI_CheckExcitement_Positive_Not1stUpForMax
	score +30
	end
AI_CheckExcitement_Positive_Not1stUpForMax:
	if_random 100, AI_CheckExcitement_End
	score +10
	end
AI_CheckExcitement_Positive_Repeat:
	if_effect_not_eq CONTEST_EFFECT_REPETITION_NOT_BORING, AI_CheckExcitement_End
	if_user_order_not_eq MON_1, AI_CheckExcitement_Positive_Not1stUpForMax
	if_excitement_not_eq 4, AI_CheckExcitement_Positive_Not1stUpForMax
	score +30
	end
AI_CheckExcitement_End:
	end

AI_CheckCombo:
	if_would_finish_combo AI_CheckCombo_WouldFinish
	call AI_CheckCombo_CheckStarter
	call AI_CheckCombo_CheckFinisherWithoutStarter
	end
AI_CheckCombo_CheckStarter:
	if_move_used_count_not_eq 0, AI_CheckCombo_End
	if_not_combo_starter AI_CheckCombo_End
	if_user_order_eq MON_1, AI_CheckCombo_Starter1stUp
	if_user_order_eq MON_2, AI_CheckCombo_Starter2ndUp
	if_user_order_eq MON_3, AI_CheckCombo_Starter3rdUp
	if_user_order_eq MON_4, AI_CheckCombo_StarterLast
	end
AI_CheckCombo_CheckFinisherWithoutStarter:
	if_not_combo_finisher AI_CheckCombo_End
	score -10
	end
AI_CheckCombo_WouldFinish:
	score +25
	end
AI_CheckCombo_Starter1stUp:
	if_last_appeal AI_CheckCombo_StarterOnLastAppeal
	if_random 150, AI_CheckCombo_End
	score +10
	end
AI_CheckCombo_Starter2ndUp:
	if_last_appeal AI_CheckCombo_StarterOnLastAppeal
	if_random 125, AI_CheckCombo_End
	score +10
	end
AI_CheckCombo_Starter3rdUp:
	if_last_appeal AI_CheckCombo_StarterOnLastAppeal
	if_random 50, AI_CheckCombo_End
	score +10
	end
AI_CheckCombo_StarterLast:
	if_last_appeal AI_CheckCombo_StarterOnLastAppeal
	score +10
	end
AI_CheckCombo_StarterOnLastAppeal:
	if_random 125, AI_CheckCombo_End
	score -15
	end
AI_CheckCombo_End:
	end

AI_CheckForGoodMove:
	if_effect_eq CONTEST_EFFECT_BETTER_WITH_GOOD_CONDITION, AI_CGM_BetterWithGoodCondition
	if_effect_eq CONTEST_EFFECT_NEXT_APPEAL_EARLIER, AI_CGM_NextAppealEarlier
	if_effect_eq CONTEST_EFFECT_NEXT_APPEAL_LATER, AI_CGM_NextAppealLater
	if_effect_eq CONTEST_EFFECT_REPETITION_NOT_BORING, AI_CGM_RepetitionNotBoring
	if_effect_eq CONTEST_EFFECT_IMPROVE_CONDITION_PREVENT_NERVOUSNESS, AI_CGM_ImproveCondition
	if_effect_eq CONTEST_EFFECT_DONT_EXCITE_AUDIENCE, AI_CGM_DontExciteAudience
	if_effect_eq CONTEST_EFFECT_APPEAL_AS_GOOD_AS_PREV_ONES, AI_CGM_AppealAsGoodAsPrevOnes
	if_effect_eq CONTEST_EFFECT_APPEAL_AS_GOOD_AS_PREV_ONE, AI_CGM_AppealAsGoodAsPrevOne
	if_effect_eq CONTEST_EFFECT_BETTER_WHEN_AUDIENCE_EXCITED, ContestEffect46
	if_effect_eq CONTEST_EFFECT_WORSEN_CONDITION_OF_PREV_MONS, ContestEffect27
	if_effect_eq CONTEST_EFFECT_SHIFT_JUDGE_ATTENTION, ContestEffect16or17
	if_effect_eq CONTEST_EFFECT_STARTLE_MON_WITH_JUDGES_ATTENTION, ContestEffect16or17
	if_effect_eq CONTEST_EFFECT_MAKE_FOLLOWING_MONS_NERVOUS, ContestEffect_FollowingMonsNervous
	if_effect_eq CONTEST_EFFECT_JAMS_OTHERS_BUT_MISS_ONE_TURN, ContestEffect18
	end

AI_CGM_BetterWithGoodCondition:
	if_user_condition_eq 3, AI_CGM_BetterWithGoodCondition_3
	if_user_condition_eq 2, AI_CGM_BetterWithGoodCondition_2
	if_user_condition_eq 1, AI_CGM_BetterWithGoodCondition_1
	if_user_condition_eq 0, AI_CGM_BetterWithGoodCondition_0
	end
AI_CGM_BetterWithGoodCondition_3:
	score +20
	end
AI_CGM_BetterWithGoodCondition_2:
	if_random 125, ContestEffectEnd
	score +15
	end
AI_CGM_BetterWithGoodCondition_1:
	if_random 125, ContestEffectEnd
	score +5
	end
AI_CGM_BetterWithGoodCondition_0:
	score -20
	end

AI_CGM_NextAppealEarlier:
	if_effect_in_user_moveset CONTEST_EFFECT_BETTER_IF_FIRST, ContestEffectEnd
	if_random 50, ContestEffectEnd
	score +20
	end

AI_CGM_NextAppealLater:
	if_effect_in_user_moveset CONTEST_EFFECT_BETTER_IF_LAST, ContestEffectEnd
	if_random 50, ContestEffectEnd
	score +20
	end

AI_CGM_RepetitionNotBoring:
	if_user_order_not_eq MON_4, ContestEffectEnd
	if_random 50, ContestEffectEnd
	score +15
	end
AI_CGM_Unused:
	if_last_appeal AI_CGM_Unused_LastAppeal
	if_random 220, AI_CGM_Unused_Discourage
	score +10
	end
AI_CGM_Unused_LastAppeal:
	if_random 20, ContestEffectEnd
	score +15
	end
AI_CGM_Unused_Discourage:
	score -20
	end

AI_CGM_ImproveCondition:
	if_effect_in_user_moveset CONTEST_EFFECT_BETTER_WITH_GOOD_CONDITION, AI_CGM_ImproveCondition_CheckAppealNum
	if_user_condition_eq 3, AI_CGM_ImproveCondition_AtMax
	if_random 50, ContestEffectEnd
	score +15
	end
AI_CGM_ImproveCondition_AtMax:
	score -10
	end
AI_CGM_ImproveCondition_CheckAppealNum:
	if_last_appeal AI_CGM_ImproveCondition_LastAppeal
	if_appeal_num_eq 0, AI_CGM_ImproveCondition_FirstAppeal
	if_move_used_count_eq 1, ContestEffectEnd
	if_random 125, ContestEffectEnd
	score +10
	end
AI_CGM_ImproveCondition_FirstAppeal:
	if_random 100, ContestEffectEnd
	score +10
	end
AI_CGM_ImproveCondition_LastAppeal:
	score -10
	end

AI_CGM_DontExciteAudience:
	if_move_used_count_eq 1, ContestEffectEnd
	if_user_order_eq MON_1, AI_CGM_DontExciteAudience_EarlyTurn
	if_user_order_eq MON_2, AI_CGM_DontExciteAudience_EarlyTurn
	if_not_last_appeal ContestEffectEnd
	if_user_has_exciting_move ContestEffectEnd
	if_excitement_less_than 1, ContestEffectEnd
	score +10
	end
AI_CGM_DontExciteAudience_EarlyTurn:
	if_random 127, ContestEffectEnd
	score +10
	end

AI_CGM_AppealAsGoodAsPrevOnes:
	if_user_order_eq MON_2, AI_CGM_AppealAsGoodAsPrevOnes_2ndUp
	if_user_order_eq MON_3, AI_CGM_AppealAsGoodAsPrevOnes_3rdUp
	if_user_order_eq MON_4, AI_CGM_AppealAsGoodAsPrevOnes_Last
	end
AI_CGM_AppealAsGoodAsPrevOnes_2ndUp:
	score +5
	end
AI_CGM_AppealAsGoodAsPrevOnes_3rdUp:
	score +15
	end
AI_CGM_AppealAsGoodAsPrevOnes_Last:
	score +20
	end

AI_CGM_AppealAsGoodAsPrevOne:
	if_user_order_eq MON_1, AI_CGM_AppealAsGoodAsPrevOne_1stUp
	if_user_order_eq MON_2, AI_CGM_AppealAsGoodAsPrevOne_2ndUp
	if_user_order_eq MON_3, AI_CGM_AppealAsGoodAsPrevOne_3rdUp
	if_user_order_eq MON_4, AI_CGM_AppealAsGoodAsPrevOne_Last
	end
AI_CGM_AppealAsGoodAsPrevOne_1stUp:
	score -10
	end
AI_CGM_AppealAsGoodAsPrevOne_2ndUp:
	if_cannot_participate MON_1, ContestEffectEnd
	score +5
	end
AI_CGM_AppealAsGoodAsPrevOne_3rdUp:
	if_cannot_participate MON_1, AI_CGM_AppealAsGoodAsPrevOne_3rdUp_CheckMon2
	score +5
	jump AI_CGM_AppealAsGoodAsPrevOne_3rdUp_CheckMon2
	end
AI_CGM_AppealAsGoodAsPrevOne_3rdUp_CheckMon2:
	if_cannot_participate MON_2, ContestEffectEnd
	score +5
	end
AI_CGM_AppealAsGoodAsPrevOne_Last:
	if_cannot_participate MON_1, AI_CGM_AppealAsGoodAsPrevOne_Last_CheckMon2
	score +5
	jump AI_CGM_AppealAsGoodAsPrevOne_Last_CheckMon2
	end
AI_CGM_AppealAsGoodAsPrevOne_Last_CheckMon2:
	if_cannot_participate MON_2, AI_CGM_AppealAsGoodAsPrevOne_Last_CheckMon3
	score +5
	jump AI_CGM_AppealAsGoodAsPrevOne_Last_CheckMon3
	end
AI_CGM_AppealAsGoodAsPrevOne_Last_CheckMon3:
	if_cannot_participate MON_3, ContestEffectEnd
	score +5
	end

ContestEffect46:
	if_user_order_eq MON_1, ContestEffect46_05
	if_user_order_more_than MON_1, ContestEffect46_score4
	end
ContestEffect46_05:
	if_appeal_num_not_eq 0, ContestEffect46_score1
	if_excitement_eq 4, ContestEffect46_score2
	if_excitement_eq 3, ContestEffect46_score3
	end
ContestEffect46_score1:
	if_random 125, ContestEffectEnd
	score -15
	end
ContestEffect46_score2:
	if_random 125, ContestEffectEnd
	score +20
	end
ContestEffect46_score3:
	if_random 125, ContestEffectEnd
	score +15
	end
ContestEffect46_score4:
	if_random 178, ContestEffectEnd
	score +10
	end

ContestEffect27:
	if_user_order_eq MON_1, ContestEffectEnd
	jump ContestEffect27_55_1
	end
ContestEffect27_55_1:
	if_cannot_participate MON_1, ContestEffect27_noscore
	if_condition_eq MON_1, 0, ContestEffect27_noscore
	if_condition_eq MON_1, 1, ContestEffect27_score1
	if_condition_eq MON_1, 2, ContestEffect27_score2
	if_condition_eq MON_1, 3, ContestEffect27_score3
	end
ContestEffect27_score1:
	if_random 125, ContestEffect27_55_2
	score +5
	if_user_order_more_than MON_2, ContestEffect27_55_2
	end
ContestEffect27_score2:
	if_random 125, ContestEffect27_55_2
	score +10
	if_user_order_more_than MON_2, ContestEffect27_55_2
	end
ContestEffect27_score3:
	if_random 125, ContestEffect27_55_2
	score +15
	if_user_order_more_than MON_2, ContestEffect27_55_2
	end
ContestEffect27_noscore:
	if_user_order_more_than MON_2, ContestEffect27_55_2
	end
ContestEffect27_55_2:
	if_cannot_participate MON_2, ContestEffect27_noscore2
	if_condition_eq MON_2, 0, ContestEffect27_noscore2
	if_condition_eq MON_2, 1, ContestEffect27_score4
	if_condition_eq MON_2, 2, ContestEffect27_score5
	if_condition_eq MON_2, 3, ContestEffect27_score6
	end
ContestEffect27_score4:
	if_random 125, ContestEffect27_55_3
	score +5
	if_user_order_more_than MON_3, ContestEffect27_55_3
	end
ContestEffect27_score5:
	if_random 125, ContestEffect27_55_3
	score +10
	if_user_order_more_than MON_3, ContestEffect27_55_3
	end
ContestEffect27_score6:
	if_random 125, ContestEffect27_55_3
	score +15
	if_user_order_more_than MON_3, ContestEffect27_55_3
	end
ContestEffect27_noscore2:
	if_user_order_more_than MON_3, ContestEffect27_55_3
	end
ContestEffect27_55_3:
	if_cannot_participate MON_3, ContestEffect27_end
	if_condition_eq MON_3, 0, ContestEffect27_end
	if_condition_eq MON_3, 1, ContestEffect27_score7
	if_condition_eq MON_3, 2, ContestEffect27_score8
	if_condition_eq MON_3, 3, ContestEffect27_score9
	end
ContestEffect27_score7:
	if_random 125, ContestEffectEnd
	score +5
	end
ContestEffect27_score8:
	if_random 125, ContestEffectEnd
	score +10
	end
ContestEffect27_score9:
	if_random 125, ContestEffectEnd
	score +15
	end
ContestEffect27_end:
	end

ContestEffect16or17:
	if_user_order_eq MON_1, ContestEffectEnd
	jump ContestEffect16or17_55
	end
ContestEffect16or17_55:
	if_cannot_participate MON_1, ContestEffect16or17_0E_1
	if_used_combo_starter_eq MON_1, TRUE, ContestEffect16or17_0E_1
	if_random 125, ContestEffect16or17_0E_1
	score +2
	contest_58 MON_1, ContestEffect16or17_0E_1
	score +8
	end
ContestEffect16or17_0E_1:
	if_user_order_eq MON_2, ContestEffectEnd
	if_cannot_participate MON_2, ContestEffect16or17_0E_2
	if_used_combo_starter_eq MON_2, TRUE, ContestEffect16or17_0E_2
	if_random 125, ContestEffect16or17_0E_2
	score +2
	contest_58 MON_2, ContestEffect16or17_0E_2
	score +8
	end
ContestEffect16or17_0E_2:
	if_user_order_eq MON_3, ContestEffectEnd
	if_cannot_participate MON_3, ContestEffectEnd
	if_used_combo_starter_eq MON_3, TRUE, ContestEffectEnd
	if_random 125, ContestEffectEnd
	score +2
	contest_58 MON_3, ContestEffectEnd
	score +8
	end

ContestEffect_FollowingMonsNervous:
	if_user_order_eq MON_4, ContestEffectEnd
	jump ContestEffect_FollowingMonsNervous_CheckMon4
	end
ContestEffect_FollowingMonsNervous_CheckMon4:
	if_cannot_participate MON_4, ContestEffect_FollowingMonsNervous_CheckMon3
	if_used_combo_starter_eq MON_4, FALSE, ContestEffect_FollowingMonsNervous_CheckMon3
	score +5
	if_random 125, ContestEffect16or17_0E_1
	score +5
	end
ContestEffect_FollowingMonsNervous_CheckMon3:
	if_user_order_eq MON_3, ContestEffectEnd
	if_cannot_participate MON_3, ContestEffect_FollowingMonsNervous_CheckMon2
	if_used_combo_starter_eq MON_3, FALSE, ContestEffect_FollowingMonsNervous_CheckMon2
	score +5
	if_random 125, ContestEffect16or17_0E_2
	score +5
	end
ContestEffect_FollowingMonsNervous_CheckMon2:
	if_user_order_eq MON_2, ContestEffectEnd
	if_cannot_participate MON_2, ContestEffectEnd
	if_used_combo_starter_eq MON_2, FALSE, ContestEffectEnd
	score +5
	if_random 125, ContestEffectEnd
	score +5
	end

ContestEffect18:
	if_last_appeal ContestEffect18_score1
	jump ContestEffect18_0E
	end
ContestEffect18_score1:
	score +5
	jump ContestEffect18_0E
	end
ContestEffect18_0E:
	if_user_order_eq MON_1, ContestEffect18_score2
	if_user_order_eq MON_2, ContestEffect18_random1
	if_user_order_eq MON_3, ContestEffect18_random2
	if_user_order_eq MON_4, ContestEffect18_random3
	end
ContestEffect18_score2:
	score -15
	end
ContestEffect18_random1:
	if_random 125, ContestEffectEnd
	score -10
	end
ContestEffect18_random2:
	if_random 125, ContestEffectEnd
	score +5
	end
ContestEffect18_random3:
	if_random 125, ContestEffectEnd
	score +15
	end

ContestEffectEnd:
	end

@ Randomly encourage moves in Cute, Smart, and Tough contests.
AI_Erratic:
	if_contest_type_eq CONTEST_CUTE, Erratic_CuteSmartTough
	if_contest_type_eq CONTEST_SMART, Erratic_CuteSmartTough
	if_contest_type_eq CONTEST_TOUGH, Erratic_CuteSmartTough
	end
Erratic_CuteSmartTough:
	if_random 125, Erratic_NoScoreIncrease
	score +10
	end
Erratic_NoScoreIncrease:
	end

AI_CheckForBadMove:
	if_effect_eq CONTEST_EFFECT_STARTLE_FRONT_MON, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_STARTLE_PREV_MON, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_BADLY_STARTLE_FRONT_MON, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_STARTLE_PREV_MON_2, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_APPEAL_AS_GOOD_AS_PREV_ONE, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_BETTER_IF_SAME_TYPE, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_BETTER_IF_DIFF_TYPE, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_AFFECTED_BY_PREV_APPEAL, ContestEffect2_8
	if_effect_eq CONTEST_EFFECT_SLIGHTLY_STARTLE_PREV_MONS, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_PREV_MONS, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_BADLY_STARTLE_PREV_MONS, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_PREV_MONS_2, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_MON_WITH_JUDGES_ATTENTION, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_SHIFT_JUDGE_ATTENTION, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_JAMS_OTHERS_BUT_MISS_ONE_TURN, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_MONS_SAME_TYPE_APPEAL, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_BADLY_STARTLE_MONS_WITH_GOOD_APPEALS, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_MONS_COOL_APPEAL, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_MONS_BEAUTY_APPEAL, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_MONS_CUTE_APPEAL, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_MONS_SMART_APPEAL, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_STARTLE_MONS_TOUGH_APPEAL, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_BADLY_STARTLES_MONS_IN_GOOD_CONDITION, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_WORSEN_CONDITION_OF_PREV_MONS, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_APPEAL_AS_GOOD_AS_PREV_ONES, ContestEffect2_9
	if_effect_eq CONTEST_EFFECT_MAKE_FOLLOWING_MON_NERVOUS, ContestEffect2_25
	if_effect_eq CONTEST_EFFECT_MAKE_FOLLOWING_MONS_NERVOUS, ContestEffect2_26
	if_effect_eq CONTEST_EFFECT_DONT_EXCITE_AUDIENCE, ContestEffect2_26
	if_effect_eq CONTEST_EFFECT_IMPROVE_CONDITION_PREVENT_NERVOUSNESS, ContestEffect2_38
	if_effect_eq CONTEST_EFFECT_AVOID_STARTLE_ONCE, ContestEffect2_4
	if_effect_eq CONTEST_EFFECT_AVOID_STARTLE, ContestEffect2_4
	if_effect_eq CONTEST_EFFECT_AVOID_STARTLE_SLIGHTLY, ContestEffect2_4
	if_effect_eq CONTEST_EFFECT_GREAT_APPEAL_BUT_NO_MORE_MOVES, ContestEffect2_2
	end

ContestEffect2_8:
	if_user_order_eq MON_1, ContestEffect2_8_score1
	if_user_order_eq MON_2, ContestEffect2_8_score2
	if_user_order_eq MON_3, ContestEffect2_8_score3
	if_user_order_eq MON_4, ContestEffect2_8_score4
	end
ContestEffect2_8_score1:
	score -10
	end
ContestEffect2_8_score2:
	if_can_participate MON_1, ContestEffectEnd2
	score -10
	end
ContestEffect2_8_score3:
	if_can_participate MON_2, ContestEffectEnd2
	score -10
	end
ContestEffect2_8_score4:
	if_can_participate MON_3, ContestEffectEnd2
	score -10
	end

ContestEffect2_9:
	if_user_order_eq MON_1, ContestEffect2_9_score1
	if_user_order_eq MON_2, ContestEffect2_9_score2
	if_user_order_eq MON_3, ContestEffect2_9_score3
	if_user_order_eq MON_4, ContestEffect2_9_score4
	end
ContestEffect2_9_score1:
	score -20
	end
ContestEffect2_9_score2:
	if_can_participate MON_1, ContestEffectEnd2
	score -15
	end
ContestEffect2_9_score3:
	if_can_participate MON_1, ContestEffectEnd2
	if_can_participate MON_2, ContestEffectEnd2
	score -15
	end
ContestEffect2_9_score4:
	if_can_participate MON_1, ContestEffectEnd2
	if_can_participate MON_2, ContestEffectEnd2
	if_can_participate MON_3, ContestEffectEnd2
	score -15
	end

ContestEffect2_25:
	if_user_order_eq MON_1, ContestEffect2_25_score1
	if_user_order_eq MON_2, ContestEffect2_25_score2
	if_user_order_eq MON_3, ContestEffect2_25_score3
	score -10
	end
ContestEffect2_25_score1:
	if_can_participate MON_2, ContestEffectEnd2
	score -10
	end
ContestEffect2_25_score2:
	if_can_participate MON_3, ContestEffectEnd2
	score -10
	end
ContestEffect2_25_score3:
	if_can_participate MON_4, ContestEffectEnd2
	score -10
	end

ContestEffect2_26:
	if_user_order_eq MON_1, ContestEffect2_26_score1
	if_user_order_eq MON_2, ContestEffect2_26_score2
	if_user_order_eq MON_3, ContestEffect2_26_score3
	score -10
	end
ContestEffect2_26_score1:
	if_can_participate MON_2, ContestEffectEnd2
	if_can_participate MON_3, ContestEffectEnd2
	if_can_participate MON_4, ContestEffectEnd2
	score -10
	end
ContestEffect2_26_score2:
	if_can_participate MON_3, ContestEffectEnd2
	if_can_participate MON_4, ContestEffectEnd2
	score -10
	end
ContestEffect2_26_score3:
	if_can_participate MON_4, ContestEffectEnd2
	score -10
	end

ContestEffect2_38:
	if_user_condition_less_than 3, ContestEffectEnd2
	score -20
	end

ContestEffect2_4:
	if_user_order_eq MON_1, ContestEffect2_4_score1
	if_user_order_eq MON_2, ContestEffect2_4_score2
	if_user_order_eq MON_3, ContestEffect2_4_score3
	score -10
	end
ContestEffect2_4_score1:
	if_can_participate MON_2, ContestEffectEnd2
	if_can_participate MON_3, ContestEffectEnd2
	if_can_participate MON_4, ContestEffectEnd2
	score -10
	end
ContestEffect2_4_score2:
	if_can_participate MON_3, ContestEffectEnd2
	if_can_participate MON_4, ContestEffectEnd2
	score -10
	end
ContestEffect2_4_score3:
	if_can_participate MON_4, ContestEffectEnd2
	score -10
	end

ContestEffect2_2:
	if_appeal_num_eq 0, ContestEffect2_2_score1
	if_appeal_num_eq 1, ContestEffect2_2_score2
	if_appeal_num_eq 2, ContestEffect2_2_score3
	if_appeal_num_eq 3, ContestEffect2_2_score4
	if_last_appeal ContestEffect2_2_score5
	end
ContestEffect2_2_score1:
	if_random 20, ContestEffectEnd2
	score -15
	end
ContestEffect2_2_score2:
	if_random 40, ContestEffectEnd2
	score -15
	end
ContestEffect2_2_score3:
	if_random 60, ContestEffectEnd2
	score -15
	end
ContestEffect2_2_score4:
	if_random 80, ContestEffectEnd2
	score -15
	end
ContestEffect2_2_score5:
	if_random 20, ContestEffectEnd2
	score +20
	end

ContestEffectEnd2:
	end

AI_CheckOrder:
	if_user_order_eq MON_1, AI_effectcheck1_081DCA4C
	if_user_order_eq MON_2, AI_effectcheck2_081DCA4C
	if_user_order_eq MON_3, AI_effectcheck3_081DCA4C
	if_user_order_eq MON_4, AI_effectcheck4_081DCA4C
	end
AI_effectcheck1_081DCA4C:
	if_effect_eq CONTEST_EFFECT_BETTER_IF_FIRST, AI_score1_081DCA4C
	if_effect_eq CONTEST_EFFECT_BETTER_WHEN_LATER, AI_score2_081DCA4C
	if_effect_type_eq CONTEST_EFFECT_TYPE_AVOID_STARTLE, AI_random1_081DCA4C
	end
AI_score1_081DCA4C:
	score +15
	end
AI_score2_081DCA4C:
	score -15
	end
AI_random1_081DCA4C:
	if_random 100, ContestEffectEnd2
	score +10
	end
AI_effectcheck2_081DCA4C:
	if_effect_eq CONTEST_EFFECT_BETTER_WHEN_LATER, AI_score3_081DCA4C
	if_effect_type_eq CONTEST_EFFECT_TYPE_AVOID_STARTLE, AI_random2_081DCA4C
	end
AI_score3_081DCA4C:
	score -5
	end
AI_random2_081DCA4C:
	if_random 125, ContestEffectEnd2
	score +10
	end
AI_effectcheck3_081DCA4C:
	if_effect_eq CONTEST_EFFECT_BETTER_WHEN_LATER, AI_score4_081DCA4C
	if_effect_eq CONTEST_EFFECT_APPEAL_AS_GOOD_AS_PREV_ONES, AI_score4_081DCA4C
	if_effect_eq CONTEST_EFFECT_USER_MORE_EASILY_STARTLED, AI_score4_081DCA4C
	end
AI_score4_081DCA4C:
	score +5
	end
AI_effectcheck4_081DCA4C:
	if_effect_eq CONTEST_EFFECT_BETTER_WHEN_LATER, AI_score5_081DCA4C
	if_effect_eq CONTEST_EFFECT_BETTER_IF_LAST, AI_score5_081DCA4C
	if_effect_eq CONTEST_EFFECT_APPEAL_AS_GOOD_AS_PREV_ONES, AI_score5_081DCA4C
	if_effect_eq CONTEST_EFFECT_USER_MORE_EASILY_STARTLED, AI_score5_081DCA4C
	if_effect_eq CONTEST_EFFECT_JAMS_OTHERS_BUT_MISS_ONE_TURN, AI_score7_081DCA4C
	if_effect_type_eq CONTEST_EFFECT_TYPE_AVOID_STARTLE, AI_score6_081DCA4C
	if_effect_type_eq CONTEST_EFFECT_TYPE_STARTLE_MONS, AI_random3_081DCA4C
	end
AI_score5_081DCA4C:
	score +15
	end
AI_score6_081DCA4C:
	score -10
	end
AI_random3_081DCA4C:
	if_random 125, ContestEffectEnd2
	score +10
	end
AI_score7_081DCA4C:
	score +5
	end

AI_Nothing:
	end
