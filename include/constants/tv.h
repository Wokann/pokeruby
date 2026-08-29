#ifndef GUARD_CONSTANTS_TV_H
#define GUARD_CONSTANTS_TV_H

#define PLAYERS_HOUSE_TV_NONE  0
#define PLAYERS_HOUSE_TV_LATI  1
#define PLAYERS_HOUSE_TV_MOVIE 2

// Event scripts cannot include tv.h, which owns the C enum.
#ifndef GUARD_TV_H
#define TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE      6
#define TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE 7
#endif  // GUARD_TV_H

// TV Show states for Bravo Trainer's Battle Tower interview
#define BRAVOTOWER_STATE_INTRO                 0
#define BRAVOTOWER_STATE_NEW_RECORD            1
#define BRAVOTOWER_STATE_LOST                  2
#define BRAVOTOWER_STATE_WON                   3
#define BRAVOTOWER_STATE_LOST_FINAL            4
#define BRAVOTOWER_STATE_SATISFIED             5
#define BRAVOTOWER_STATE_UNSATISFIED           6
#define BRAVOTOWER_STATE_UNUSED_1              7
#define BRAVOTOWER_STATE_UNUSED_2              8
#define BRAVOTOWER_STATE_UNUSED_3              9
#define BRAVOTOWER_STATE_UNUSED_4             10
#define BRAVOTOWER_STATE_RESPONSE             11
#define BRAVOTOWER_STATE_RESPONSE_SATISFIED   12
#define BRAVOTOWER_STATE_RESPONSE_UNSATISFIED 13
#define BRAVOTOWER_STATE_OUTRO                14

#endif // GUARD_CONSTANTS_TV_H
