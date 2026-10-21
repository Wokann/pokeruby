#ifndef GUARD_CONSTANTS_CABLE_CLUB_H
#define GUARD_CONSTANTS_CABLE_CLUB_H

// States for VAR_CABLE_CLUB_STATE
#define USING_SINGLE_BATTLE  1
#define USING_DOUBLE_BATTLE  2
#define USING_TRADE_CENTER   3
#define USING_RECORD_CORNER  4
#define USING_MULTI_BATTLE   5

// Return states for the group of specials that use CreateLinkupTask
#define LINKUP_ONGOING              0
#define LINKUP_SUCCESS              1
#define LINKUP_SOMEONE_NOT_READY    2
#define LINKUP_DIFF_SELECTIONS      3
#define LINKUP_WRONG_NUM_PLAYERS    4
#define LINKUP_FAILED               5
#define LINKUP_CONNECTION_ERROR     6
// Ruby/Sapphire use 7 for foreign-game mixing failure; Emerald uses 12.
#define LINKUP_FOREIGN_GAME         7

#define CABLE_SEAT_WAITING 0
#define CABLE_SEAT_SUCCESS 1
#define CABLE_SEAT_FAILED  2

#endif // GUARD_CONSTANTS_CABLE_CLUB_H
