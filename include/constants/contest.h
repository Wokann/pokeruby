#ifndef GUARD_CONSTANTS_CONTEST_H
#define GUARD_CONSTANTS_CONTEST_H

// Event scripts use these aliases for the values enumerated in contest.h.
#ifndef GUARD_CONTEST_H
#define CONTEST_CATEGORY_COOL   0
#define CONTEST_CATEGORY_BEAUTY 1
#define CONTEST_CATEGORY_CUTE   2
#define CONTEST_CATEGORY_SMART  3
#define CONTEST_CATEGORY_TOUGH  4
#define CONTEST_CATEGORIES_COUNT 5

#define CONTEST_RANK_NORMAL 0
#define CONTEST_RANK_SUPER  1
#define CONTEST_RANK_HYPER  2
#define CONTEST_RANK_MASTER 3
#define CONTEST_RANK_LINK   4

#define CONTEST_WINNER_ARTIST        0
#define CONTEST_WINNER_NORMAL        1
#define CONTEST_WINNER_SUPER         2
#define CONTEST_WINNER_HYPER_1       3
#define CONTEST_WINNER_HYPER_2       4
#define CONTEST_WINNER_HYPER_3       5
#define CONTEST_WINNER_MASTER_1      6
#define CONTEST_WINNER_MASTER_2      7
#define CONTEST_WINNER_MASTER_3      8
#define CONTEST_WINNER_MUSEUM_COOL   9
#define CONTEST_WINNER_MUSEUM_BEAUTY 10
#define CONTEST_WINNER_MUSEUM_CUTE   11
#define CONTEST_WINNER_MUSEUM_SMART  12
#define CONTEST_WINNER_MUSEUM_TOUGH  13

#define CANT_ENTER_CONTEST           0
#define CAN_ENTER_CONTEST_EQUAL_RANK 1
#define CAN_ENTER_CONTEST_HIGH_RANK  2
#define CANT_ENTER_CONTEST_EGG       3
#define CANT_ENTER_CONTEST_FAINTED   4

#define CONTEST_LOCATION_NONE       0
#define CONTEST_LOCATION_VERDANTURF 1
#define CONTEST_LOCATION_FALLARBOR  2
#define CONTEST_LOCATION_SLATEPORT  3
#define CONTEST_LOCATION_LILYCOVE   4
#define CONTEST_LOCATION_LINK       5

#define LILYCOVE_CONTEST_LOBBY_STATE_NONE                0
#define LILYCOVE_CONTEST_LOBBY_STATE_CONTEST_ARTIST      1
#define LILYCOVE_CONTEST_LOBBY_STATE_LINK_CONTEST_ARTIST 2
#endif  // GUARD_CONTEST_H

#define CONTEST_AI_CHECK_BAD_MOVE   (1 << 0)
#define CONTEST_AI_CHECK_COMBO      (1 << 1)
#define CONTEST_AI_CHECK_BORING     (1 << 2)
#define CONTEST_AI_CHECK_EXCITEMENT (1 << 3)
#define CONTEST_AI_CHECK_ORDER      (1 << 4)
#define CONTEST_AI_CHECK_GOOD_MOVE  (1 << 5)
#define CONTEST_AI_ERRATIC          (1 << 6)
#define CONTEST_AI_DUMMY_1          (1 << 7)
#define CONTEST_AI_DUMMY_2          (1 << 8)
#define CONTEST_AI_DUMMY_3          (1 << 9)
#define CONTEST_AI_DUMMY_4          (1 << 10)
#define CONTEST_AI_DUMMY_5          (1 << 11)
#define CONTEST_AI_DUMMY_6          (1 << 12)
#define CONTEST_AI_DUMMY_7          (1 << 13)
#define CONTEST_AI_DUMMY_8          (1 << 14)
#define CONTEST_AI_DUMMY_9          (1 << 15)
#define CONTEST_AI_DUMMY_10         (1 << 16)
#define CONTEST_AI_DUMMY_11         (1 << 17)
#define CONTEST_AI_DUMMY_12         (1 << 18)
#define CONTEST_AI_DUMMY_13         (1 << 19)
#define CONTEST_AI_DUMMY_14         (1 << 20)
#define CONTEST_AI_DUMMY_15         (1 << 21)
#define CONTEST_AI_DUMMY_16         (1 << 22)
#define CONTEST_AI_DUMMY_17         (1 << 23)
#define CONTEST_AI_DUMMY_18         (1 << 24)
#define CONTEST_AI_DUMMY_19         (1 << 25)
#define CONTEST_AI_DUMMY_20         (1 << 26)
#define CONTEST_AI_DUMMY_21         (1 << 27)
#define CONTEST_AI_DUMMY_22         (1 << 28)
#define CONTEST_AI_DUMMY_23         (1 << 29)
#define CONTEST_AI_DUMMY_24         (1 << 30)
#define CONTEST_AI_DUMMY_25         (1 << 31)

// The below scripts are used by every AI contest opponent
// It includes every non-dummy script
#define CONTEST_AI_COMMON   (CONTEST_AI_CHECK_BAD_MOVE | CONTEST_AI_CHECK_COMBO | CONTEST_AI_CHECK_BORING | \
                             CONTEST_AI_CHECK_EXCITEMENT | CONTEST_AI_CHECK_ORDER | CONTEST_AI_CHECK_GOOD_MOVE | CONTEST_AI_ERRATIC | \
                             CONTEST_AI_DUMMY_1 | CONTEST_AI_DUMMY_2 | CONTEST_AI_DUMMY_3 | CONTEST_AI_DUMMY_4 | CONTEST_AI_DUMMY_5)

#endif  // GUARD_CONSTANTS_CONTEST_H
