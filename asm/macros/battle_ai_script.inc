	.macro if_random_less_than percent, address
	.byte 0x00
	.byte \percent
	.4byte \address
	.endm

	@ unused
	.macro if_random_greater_than percent, address
	.byte 0x01
	.byte \percent
	.4byte \address
	.endm

	@ unused
	.macro if_random_equal address
	.byte 0x02
	.4byte \address
	.endm

	@ unused
	.macro if_random_not_equal address
	.byte 0x03
	.4byte \address
	.endm

	.macro score score
	.byte 0x04
	.byte \score
	.endm

	.macro if_hp_less_than battler, percent, address
	.byte 0x05
	.byte \battler
	.byte \percent
	.4byte \address
	.endm

	.macro if_hp_more_than battler, percent, address
	.byte 0x06
	.byte \battler
	.byte \percent
	.4byte \address
	.endm

	.macro if_hp_equal battler, percent, address
	.byte 0x07
	.byte \battler
	.byte \percent
	.4byte \address
	.endm

	.macro if_hp_not_equal battler, percent, address
	.byte 0x08
	.byte \battler
	.byte \percent
	.4byte \address
	.endm

	.macro if_status battler, status1, address
	.byte 0x09
	.byte \battler
	.4byte \status1
	.4byte \address
	.endm

	.macro if_not_status battler, status1, address
	.byte 0x0a
	.byte \battler
	.4byte \status1
	.4byte \address
	.endm

	.macro if_status2 battler, status2, address
	.byte 0x0b
	.byte \battler
	.4byte \status2
	.4byte \address
	.endm

	.macro if_not_status2 battler, status2, address
	.byte 0x0c
	.byte \battler
	.4byte \status2
	.4byte \address
	.endm

	.macro if_status3 battler, status3, address
	.byte 0x0d
	.byte \battler
	.4byte \status3
	.4byte \address
	.endm

	.macro if_not_status3 battler, status3, address
	.byte 0x0e
	.byte \battler
	.4byte \status3
	.4byte \address
	.endm

	.macro if_side_affecting battler, sidestatus, address
	.byte 0x0f
	.byte \battler
	.4byte \sidestatus
	.4byte \address
	.endm

	.macro if_not_side_affecting battler, sidestatus, address
	.byte 0x10
	.byte \battler
	.4byte \sidestatus
	.4byte \address
	.endm

	.macro if_less_than value, address
	.byte 0x11
	.byte \value
	.4byte \address
	.endm

	.macro if_more_than value, address
	.byte 0x12
	.byte \value
	.4byte \address
	.endm

	.macro if_equal value, address
	.byte 0x13
	.byte \value
	.4byte \address
	.endm

	.macro if_not_equal value, address
	.byte 0x14
	.byte \value
	.4byte \address
	.endm

	.macro if_less_than_ptr value, address
	.byte 0x15
	.4byte \value
	.4byte \address
	.endm

	.macro if_more_than_ptr value, address
	.byte 0x16
	.4byte \value
	.4byte \address
	.endm

	.macro if_equal_ptr value, address
	.byte 0x17
	.4byte \value
	.4byte \address
	.endm

	.macro if_not_equal_ptr value, address
	.byte 0x18
	.4byte \value
	.4byte \address
	.endm

	.macro if_move move, address
	.byte 0x19
	.2byte \move
	.4byte \address
	.endm

	.macro if_not_move move, address
	.byte 0x1a
	.2byte \move
	.4byte \address
	.endm

	.macro if_in_bytes list, address
	.byte 0x1b
	.4byte \list
	.4byte \address
	.endm

	.macro if_not_in_bytes list, address
	.byte 0x1c
	.4byte \list
	.4byte \address
	.endm

	.macro if_in_hwords list, address
	.byte 0x1d
	.4byte \list
	.4byte \address
	.endm

	.macro if_not_in_hwords list, address
	.byte 0x1e
	.4byte \list
	.4byte \address
	.endm

	.macro if_user_has_attacking_move address
	.byte 0x1f
	.4byte \address
	.endm

	.macro if_user_has_no_attacking_moves address
	.byte 0x20
	.4byte \address
	.endm

	.macro get_turn_count
	.byte 0x21
	.endm

	.macro get_type byte
	.byte 0x22
	.byte \byte
	.endm

	@ unused
	.macro get_considered_move_power
	.byte 0x23
	.endm

	.macro get_how_powerful_move_is
	.byte 0x24
	.endm

	.macro get_last_used_bank_move battler
	.byte 0x25
	.byte \battler
	.endm

	.macro if_equal_ type, address
	.byte 0x26
	.byte \type
	.4byte \address
	.endm

	@ unused
	.macro if_not_equal_ type, address
	.byte 0x27
	.byte \type
	.4byte \address
	.endm

	.macro if_user_goes target, address
	.byte 0x28
	.byte \target
	.4byte \address
	.endm

	.macro if_user_doesnt_go target, address
	.byte 0x29
	.byte \target
	.4byte \address
	.endm

	@ nullsub
	.macro nop_2A
	.byte 0x2a
	.endm

	@ nullsub
	.macro nop_2B
	.byte 0x2b
	.endm

	.macro count_usable_party_mons battler
	.byte 0x2c
	.byte \battler
	.endm

	@ unused
	.macro get_considered_move
	.byte 0x2d
	.endm

	.macro get_considered_move_effect
	.byte 0x2e
	.endm

	.macro get_ability battler
	.byte 0x2f
	.byte \battler
	.endm

	@ unused
	.macro get_highest_type_effectiveness
	.byte 0x30
	.endm

	.macro if_type_effectiveness value, address
	.byte 0x31
	.byte \value
	.4byte \address
	.endm

	@ nullsub
	.macro nop_32
	.byte 0x32
	.endm

	@ nullsub
	.macro nop_33
	.byte 0x33
	.endm

	.macro if_status_in_party battler, status1, address
	.byte 0x34
	.byte \battler
	.4byte \status1
	.4byte \address
	.endm

	@ bugged
	.macro if_status_not_in_party battler, status1, address
	.byte 0x35
	.byte \battler
	.4byte \status1
	.4byte \address
	.endm

	.macro get_weather
	.byte 0x36
	.endm

	.macro if_effect byte, address
	.byte 0x37
	.byte \byte
	.4byte \address
	.endm

	.macro if_not_effect byte, address
	.byte 0x38
	.byte \byte
	.4byte \address
	.endm

	.macro if_stat_level_less_than battler, stat, level, address
	.byte 0x39
	.byte \battler
	.byte \stat
	.byte \level
	.4byte \address
	.endm

	.macro if_stat_level_more_than battler, stat, level, address
	.byte 0x3a
	.byte \battler
	.byte \stat
	.byte \level
	.4byte \address
	.endm

	.macro if_stat_level_equal battler, stat, level, address
	.byte 0x3b
	.byte \battler
	.byte \stat
	.byte \level
	.4byte \address
	.endm

	.macro if_stat_level_not_equal battler, stat, level, address
	.byte 0x3c
	.byte \battler
	.byte \stat
	.byte \level
	.4byte \address
	.endm

	.macro if_can_faint address
	.byte 0x3d
	.4byte \address
	.endm

	.macro if_cant_faint address
	.byte 0x3e
	.4byte \address
	.endm

	@ unused
	.macro if_has_move, battler, move, address
	.byte 0x3f
	.byte \battler
	.2byte \move
	.4byte \address
	.endm

	@ unused
	.macro if_doesnt_have_move, battler, move, address
	.byte 0x40
	.byte \battler
	.2byte \move
	.4byte \address
	.endm

	.macro if_has_move_with_effect battler, effect, address
	.byte 0x41
	.byte \battler
	.byte \effect
	.4byte \address
	.endm

	.macro if_doesnt_have_move_with_effect battler, effect, address
	.byte 0x42
	.byte \battler
	.byte \effect
	.4byte \address
	.endm

	.macro if_any_move_disabled_or_encored battler, byte, address
	.byte 0x43
	.byte \battler
	.byte \byte
	.4byte \address
	.endm

	.macro if_curr_move_disabled_or_encored target, address
	.byte 0x44
	.byte \target
	.4byte \address
	.endm

	.macro flee
	.byte 0x45
	.endm

	.macro if_random_safari_flee address
	.byte 0x46
	.4byte \address
	.endm

	.macro watch
	.byte 0x47
	.endm

	.macro get_hold_effect battler
	.byte 0x48
	.byte \battler
	.endm

	.macro get_gender battler
	.byte 0x49
	.byte \battler
	.endm

	.macro is_first_turn_for battler
	.byte 0x4a
	.byte \battler
	.endm

	.macro get_stockpile_count battler
	.byte 0x4b
	.byte \battler
	.endm

	.macro is_double_battle
	.byte 0x4c
	.endm

	.macro get_used_held_item battler
	.byte 0x4d
	.byte \battler
	.endm

	.macro get_move_type_from_result
	.byte 0x4e
	.endm

	.macro get_move_power_from_result
	.byte 0x4f
	.endm

	.macro get_move_effect_from_result
	.byte 0x50
	.endm

	.macro get_protect_count battler
	.byte 0x51
	.byte \battler
	.endm

	@ nullsub
	.macro nop_52
	.byte 0x52
	.endm

	@ nullsub
	.macro nop_53
	.byte 0x53
	.endm

	@ nullsub
	.macro nop_54
	.byte 0x54
	.endm

	@ nullsub
	.macro nop_55
	.byte 0x55
	.endm

	@ nullsub
	.macro nop_56
	.byte 0x56
	.endm

	@ nullsub
	.macro nop_57
	.byte 0x57
	.endm

	@ unused
	.macro call address
	.byte 0x58
	.4byte \address
	.endm

	.macro goto address
	.byte 0x59
	.4byte \address
	.endm

	.macro end
	.byte 0x5a
	.endm

	.macro if_level_cond cond, address
	.byte 0x5b
	.byte \cond
	.4byte \address
	.endm

	.macro if_user_higher_level address
	if_level_cond 0, \address
	.endm

	.macro if_target_higher_level address
	if_level_cond 1, \address
	.endm

	.macro if_equal_levels address
	if_level_cond 2, \address
	.endm

	@ unused
	.macro if_target_taunted address
	.byte 0x5c
	.4byte \address
	.endm

	.macro if_target_not_taunted address
	.byte 0x5d
	.4byte \address
	.endm

@ useful script macros

	.macro get_curr_move_type
	get_type AI_TYPE_MOVE
	.endm

	.macro get_user_type1
	get_type AI_TYPE1_USER
	.endm

	.macro get_user_type2
	get_type AI_TYPE2_USER
	.endm

	.macro get_target_type1
	get_type AI_TYPE1_TARGET
	.endm

	.macro get_target_type2
	get_type AI_TYPE2_TARGET
	.endm
