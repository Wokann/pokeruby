@ Add a positive/negative value to the score of the move being evaluated.

	.macro score score
	.byte 0x00
	.byte \score
	.endm

@ turn (AKA "Appeal No.")

	.macro get_appeal_num
	.byte 0x01
	.endm

	.macro if_appeal_num_less_than num, destination
	.byte 0x02
	.byte \num
	.4byte \destination
	.endm

	.macro if_appeal_num_more_than num, destination
	.byte 0x03
	.byte \num
	.4byte \destination
	.endm

	.macro if_appeal_num_eq num, destination
	.byte 0x04
	.byte \num
	.4byte \destination
	.endm

	.macro if_appeal_num_not_eq num, destination
	.byte 0x05
	.byte \num
	.4byte \destination
	.endm

@ audience excitement

	.macro get_excitement
	.byte 0x06
	.endm

	.macro if_excitement_less_than num, destination
	.byte 0x07
	.byte \num
	.4byte \destination
	.endm

	.macro if_excitement_more_than num, destination
	.byte 0x08
	.byte \num
	.4byte \destination
	.endm

	.macro if_excitement_eq num, destination
	.byte 0x09
	.byte \num
	.4byte \destination
	.endm

	.macro if_excitement_not_eq num, destination
	.byte 0x0A
	.byte \num
	.4byte \destination
	.endm

@ the order that the user goes in the current turn

	.macro get_user_order
	.byte 0x0B
	.endm

	.macro if_user_order_less_than num destination
	.byte 0x0C
	.byte \num
	.4byte \destination
	.endm

	.macro if_user_order_more_than num destination
	.byte 0x0D
	.byte \num
	.4byte \destination
	.endm

	.macro if_user_order_eq num destination
	.byte 0x0E
	.byte \num
	.4byte \destination
	.endm

	.macro if_user_order_not_eq num destination
	.byte 0x0F
	.byte \num
	.4byte \destination
	.endm

@ user condition

	.macro get_user_condition
	.byte 0x10
	.endm

	.macro if_user_condition_less_than num, destination
	.byte 0x11
	.byte \num
	.4byte \destination
	.endm

	.macro if_user_condition_more_than num, destination
	.byte 0x12
	.byte \num
	.4byte \destination
	.endm

	.macro if_user_condition_eq num, destination
	.byte 0x13
	.byte \num
	.4byte \destination
	.endm

	.macro if_user_condition_not_eq num, destination
	.byte 0x14
	.byte \num
	.4byte \destination
	.endm

@ 15
@ 16
@ 17
@ 18
@ 19
@ 1A
@ 1B
@ 1C
@ 1D
@ 1E

@ contest type

	.macro get_contest_type
	.byte 0x1F
	.endm

	.macro if_contest_type_eq type, destination
	.byte 0x20
	.byte \type
	.4byte \destination
	.endm

	.macro if_contest_type_not_eq type, destination
	.byte 0x21
	.byte \type
	.4byte \destination
	.endm

@ move excitement (change in excitement due to move)

	.macro get_move_excitement
	.byte 0x22
	.endm

	.macro if_move_excitement_less_than num, destination
	.byte 0x23
	.byte \num
	.4byte \destination
	.endm

	.macro if_move_excitement_more_than num, destination
	.byte 0x24
	.byte \num
	.4byte \destination
	.endm

	.macro if_move_excitement_eq num, destination
	.byte 0x25
	.byte \num
	.4byte \destination
	.endm

	.macro if_move_excitement_not_eq num, destination
	.byte 0x26
	.byte \num
	.4byte \destination
	.endm

@ move effect

	.macro get_effect
	.byte 0x27
	.endm

	.macro if_effect_eq effect, destination
	.byte 0x28
	.byte \effect
	.4byte \destination
	.endm

	.macro if_effect_not_eq effect, destination
	.byte 0x29
	.byte \effect
	.4byte \destination
	.endm

@ move effect type

	.macro get_effect_type
	.byte 0x2A
	.endm

	.macro if_effect_type_eq type, destination
	.byte 0x2B
	.byte \type
	.4byte \destination
	.endm

	.macro if_effect_type_not_eq type, destination
	.byte 0x2C
	.byte \type
	.4byte \destination
	.endm

@ whether the current move is the most appealing in the user's moveset

	.macro check_most_appealing_move
	.byte 0x2D
	.endm

	.macro if_most_appealing_move destination
	.byte 0x2E
	.4byte \destination
	.endm

@ 2F
@ 30
@ 31
@ 32
@ 33
@ 34
@ 35
@ 36
@ 37
@ 38
@ 39
@ 3A

@ number of times current move has been used

	.macro get_move_used_count
	.byte 0x3B
	.endm

	.macro if_move_used_count_less_than num, destination
	.byte 0x3C
	.byte \num
	.4byte \destination
	.endm

	.macro if_move_used_count_more_than num, destination
	.byte 0x3D
	.byte \num
	.4byte \destination
	.endm

	.macro if_move_used_count_eq num, destination
	.byte 0x3E
	.byte \num
	.4byte \destination
	.endm

	.macro if_move_used_count_not_eq num, destination
	.byte 0x3F
	.byte \num
	.4byte \destination
	.endm

@ whether the current move is a combo starter (with another move in the moveset)

	.macro check_combo_starter
	.byte 0x40
	.endm

	.macro if_combo_starter destination
	.byte 0x41
	.4byte \destination
	.endm

	.macro if_not_combo_starter destination
	.byte 0x42
	.4byte \destination
	.endm

@ whether the current move is a combo finisher (with another move in the moveset)

	.macro check_combo_finisher
	.byte 0x43
	.endm

	.macro if_combo_finisher destination
	.byte 0x44
	.4byte \destination
	.endm

	.macro if_not_combo_finisher destination
	.byte 0x45
	.4byte \destination
	.endm

@ whether the current move would finish a combo

	.macro check_would_finish_combo
	.byte 0x46
	.endm

	.macro if_would_finish_combo destination
	.byte 0x47
	.4byte \destination
	.endm

	.macro if_would_not_finish_combo destination
	.byte 0x48
	.4byte \destination
	.endm

@ condition of mon (indexed by order)

	.macro get_condition mon
	.byte 0x49
	.byte \mon
	.endm

	.macro if_condition_less_than mon, num, destination
	.byte 0x4A
	.byte \mon
	.byte \num
	.4byte \destination
	.endm

	.macro if_condition_more_than mon, num, destination
	.byte 0x4B
	.byte \mon
	.byte \num
	.4byte \destination
	.endm

	.macro if_condition_eq mon, num, destination
	.byte 0x4C
	.byte \mon
	.byte \num
	.4byte \destination
	.endm

	.macro if_condition_not_eq mon, num, destination
	.byte 0x4D
	.byte \mon
	.byte \num
	.4byte \destination
	.endm

@ whether the mon used a combo starter move
@ Even though this value is always 1 or 0 (i.e. TRUE/FALSE),
@ there are less-than and greater-than comparison operations for some reason.

	.macro get_used_combo_starter mon
	.byte 0x4E
	.byte \mon
	.endm

	.macro if_used_combo_starter_less_than mon, num, destination
	.byte 0x4F
	.byte \mon
	.byte \num
	.4byte \destination
	.endm

	.macro if_used_combo_starter_more_than mon, num, destination
	.byte 0x50
	.byte \mon
	.byte \num
	.4byte \destination
	.endm


	.macro if_used_combo_starter_eq mon, num, destination
	.byte 0x51
	.byte \mon
	.byte \num
	.4byte \destination
	.endm

	.macro if_used_combo_starter_not_eq mon, num, destination
	.byte 0x52
	.byte \mon
	.byte \num
	.4byte \destination
	.endm

	.macro if_used_combo_starter mon:req, destination:req
	if_used_combo_starter_eq \mon, TRUE, \destination
	.endm

	.macro if_not_used_combo_starter mon:req, destination:req
	if_used_combo_starter_eq \mon, FALSE, \destination
	.endm

@ whether the mon can make an appeal

	.macro check_can_participate mon
	.byte 0x53
	.byte \mon
	.endm

	.macro if_can_participate mon, destination
	.byte 0x54
	.byte \mon
	.4byte \destination
	.endm

	.macro if_cannot_participate mon, destination
	.byte 0x55
	.byte \mon
	.4byte \destination
	.endm

@ 56
@ 57

	.macro if_not_completed_combo mon destination
	.byte 0x58
	.byte \mon
	.4byte \destination
	.endm

@ 59
@ 5A
@ 5B
@ 5C
@ 5D
@ 5E
@ 5F
@ 60
@ 61
@ 62
@ 63
@ 64
@ 65
@ 66
@ 67
@ 68
@ 69
@ 6A
@ 6B
@ 6C
@ 6D
@ 6E
@ 6F
@ 70
@ 71
@ 72
@ 73
@ 74
@ 75
@ 76
@ 77
@ 78
@ 79
@ 7A
@ 7B
@ 7C

	.macro if_random_less_than num destination
	.byte 0x7D
	.byte \num
	.4byte \destination
	.endm

@ 7E

	.macro goto destination
	.byte 0x7F
	.4byte \destination
	.endm

	.macro call destination
	.byte 0x80
	.4byte \destination
	.endm

	.macro end
	.byte 0x81
	.endm

	.macro check_user_has_exciting_move
	.byte 0x82
	.endm

	.macro if_user_has_exciting_move destination
	.byte 0x83
	.4byte \destination
	.endm

	.macro if_user_doesnt_have_exciting_move destination
	.byte 0x84
	.4byte \destination
	.endm

@ 85
@ 86

	.macro if_user_doesnt_have_move move destination
	.byte 0x87
	.2byte \move
	.4byte \destination
	.endm
