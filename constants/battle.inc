@ status 1
.equiv SLP, 0x07
.equiv PSN, 0x08
.equiv BRN, 0x10
.equiv FRZ, 0x20
.equiv PAR, 0x40
.equiv TOX, 0x80

@ status 2
.equiv S_CONFUSED,     0x00000007
.equiv S_CONTINUE,     0x00001000
.equiv S_TEMP_TRAP,    0x0000E000 @ Wrap, Bind, etc.
.equiv S_INFATUATED,   0x000F0000
.equiv S_FOCUS_ENERGY, 0x00100000
.equiv S_SUBSTITUTE,   0x01000000
.equiv S_MEAN_LOOK,    0x04000000
.equiv S_NIGHTMARE,    0x08000000
.equiv S_CURSED,       0x10000000
.equiv S_FORESIGHT,    0x20000000
.equiv S_TORMENT,      0x80000000

@ status 3
.equiv S_LEECH_SEED,  0x00000004
.equiv S_ALWAYS_HIT,  0x00000010
.equiv S_PERISH_SONG, 0x00000020
.equiv S_ROOTED,      0x00000400
.equiv S_YAWN,        0x00001800
.equiv S_IMPRISONED,  0x00002000
.equiv S_MUD_SPORT,   0x00010000
.equiv S_WATER_SPORT, 0x00020000

@ status 4
.equiv S_REFLECT,      0x0001
.equiv S_LIGHT_SCREEN, 0x0002
.equiv S_SPIKES,       0x0010
.equiv S_SAFEGUARD,    0x0020
.equiv S_FUTURE_SIGHT, 0x0040
.equiv S_MIST,         0x0100

.equiv TARGET, 0
.equiv USER, 1

@ battle AI script operands
.equiv AI_TARGET,       TARGET
.equiv AI_USER,         USER
.equiv AI_TYPE1_TARGET, ENEMY_TYPE1
.equiv AI_TYPE2_TARGET, PLAYER_TYPE1
.equiv AI_TYPE1_USER,   ENEMY_TYPE2
.equiv AI_TYPE2_USER,   PLAYER_TYPE2
.equiv AI_TYPE_MOVE,    CURRENT_MOVE

@ battle AI type effectiveness
.equiv AI_EFFECTIVENESS_x4,   160
.equiv AI_EFFECTIVENESS_x2,   80
.equiv AI_EFFECTIVENESS_x1,   40
.equiv AI_EFFECTIVENESS_x0_5, 20
.equiv AI_EFFECTIVENESS_x0_25, 10
.equiv AI_EFFECTIVENESS_x0,   0

@ get_how_powerful_move_is results
.equiv MOVE_POWER_OTHER,        0
.equiv MOVE_NOT_MOST_POWERFUL,  1
.equiv MOVE_MOST_POWERFUL,      2

.equiv BATTLE_WEATHER_SUN, 0
.equiv BATTLE_WEATHER_RAIN, 1
.equiv BATTLE_WEATHER_SANDSTORM, 2
.equiv BATTLE_WEATHER_HAIL, 3


@ Battle animation shake types. Keep in sync with include/constants/battle_anim.h.
	enum_start
	enum SHAKE_BG_X
	enum SHAKE_BG_Y
	enum SHAKE_MON_X
	enum SHAKE_MON_Y

	enum_start
	enum SHAKE_MON_ATTACKER
	enum SHAKE_MON_TARGET
	enum SHAKE_MON_BOTH

	enum_start 1
	enum ATTACK
	enum DEFENSE
	enum SPEED
	enum SP_ATTACK
	enum SP_DEFENSE
	enum ACCURACY
	enum EVASION

.equiv STAT_ATK,     ATTACK
.equiv STAT_DEF,     DEFENSE
.equiv STAT_SPEED,   SPEED
.equiv STAT_SPATK,   SP_ATTACK
.equiv STAT_SPDEF,   SP_DEFENSE
.equiv STAT_ACC,     ACCURACY
.equiv STAT_EVASION, EVASION
