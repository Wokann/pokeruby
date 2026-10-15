@ Keep shared status definition names literal through CPP.

@ status 1
.equiv "STATUS1_SLEEP",              0x07
.equiv "STATUS1_POISON",             0x08
.equiv "STATUS1_BURN",               0x10
.equiv "STATUS1_FREEZE",             0x20
.equiv "STATUS1_PARALYSIS",          0x40
.equiv "STATUS1_TOXIC_POISON",       0x80

@ status 2
.equiv "STATUS2_CONFUSION",          0x00000007
.equiv "STATUS2_MULTIPLETURNS",      0x00001000
.equiv "STATUS2_WRAPPED",            0x0000E000 @ Wrap, Bind, etc.
.equiv "STATUS2_INFATUATION",        0x000F0000
.equiv "STATUS2_FOCUS_ENERGY",       0x00100000
.equiv "STATUS2_SUBSTITUTE",         0x01000000
.equiv "STATUS2_ESCAPE_PREVENTION",  0x04000000
.equiv "STATUS2_NIGHTMARE",          0x08000000
.equiv "STATUS2_CURSED",             0x10000000
.equiv "STATUS2_FORESIGHT",          0x20000000
.equiv "STATUS2_TORMENT",            0x80000000

@ status 3
.equiv "STATUS3_LEECHSEED",          0x00000004
.equiv S_ALWAYS_HIT,                 0x00000010
.equiv "STATUS3_PERISH_SONG",        0x00000020
.equiv "STATUS3_ROOTED",             0x00000400
.equiv "STATUS3_YAWN",               0x00001800
.equiv "STATUS3_IMPRISONED_OTHERS",  0x00002000
.equiv "STATUS3_MUDSPORT",           0x00010000
.equiv "STATUS3_WATERSPORT",         0x00020000

@ status 4
.equiv "SIDE_STATUS_REFLECT",        0x0001
.equiv "SIDE_STATUS_LIGHTSCREEN",    0x0002
.equiv "SIDE_STATUS_SPIKES",         0x0010
.equiv "SIDE_STATUS_SAFEGUARD",      0x0020
.equiv "SIDE_STATUS_FUTUREATTACK",   0x0040
.equiv "SIDE_STATUS_MIST",           0x0100

.equiv TARGET,                       0
.equiv USER,                         1

.equiv AI_TARGET,                    TARGET
.equiv AI_USER,                      USER

.equiv AI_WEATHER_SUN,               0
.equiv AI_WEATHER_RAIN,              1
.equiv AI_WEATHER_SANDSTORM,         2
.equiv AI_WEATHER_HAIL,              3

	enum_start 1
	enum STAT_ATK
	enum STAT_DEF
	enum STAT_SPEED
	enum STAT_SPATK
	enum STAT_SPDEF
	enum STAT_ACC
	enum STAT_EVASION
