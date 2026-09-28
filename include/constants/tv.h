#ifndef GUARD_CONSTANTS_TV_H
#define GUARD_CONSTANTS_TV_H

#define POKENEWS_NONE         0
#define POKENEWS_SLATEPORT    1
#define POKENEWS_GAME_CORNER  2
#define POKENEWS_LILYCOVE     3
#define NUM_POKENEWS_TYPES    3

#define POKENEWS_STATE_INACTIVE 0
#define POKENEWS_STATE_UPCOMING 1
#define POKENEWS_STATE_ACTIVE   2

#define POKENEWS_COUNTDOWN 4

#define TVGROUP_NONE       0
#define TVGROUP_UNUSED     1
#define TVGROUP_NORMAL     2
#define TVGROUP_RECORD_MIX 3
#define TVGROUP_OUTBREAK   4

#define TVSHOW_OFF_AIR                              0

#define TVGROUP_NORMAL_START                        1
#define TVSHOW_FAN_CLUB_LETTER                      1
#define TVSHOW_RECENT_HAPPENINGS                    2
#define TVSHOW_PKMN_FAN_CLUB_OPINIONS               3
#define TVSHOW_DUMMY                                4
#define TVSHOW_NAME_RATER_SHOW                      5
#define TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE        6
#define TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE   7
#define TVGROUP_NORMAL_END                          20

#define TVGROUP_RECORD_MIX_START                    21
#define TVSHOW_POKEMON_TODAY_CAUGHT                 21
#define TVSHOW_SMART_SHOPPER                        22
#define TVSHOW_POKEMON_TODAY_FAILED                 23
#define TVSHOW_FISHING_ADVICE                       24
#define TVSHOW_WORLD_OF_MASTERS                     25
#define TVGROUP_RECORD_MIX_END                      40

#define TVGROUP_OUTBREAK_START                      41
#define TVSHOW_MASS_OUTBREAK                        41
#define TVGROUP_OUTBREAK_END                        60

#define NUM_NORMAL_TVSHOW_SLOTS 5
#define SMARTSHOPPER_NUM_ITEMS  3

#define PLAYERS_HOUSE_TV_NONE  0
#define PLAYERS_HOUSE_TV_LATI  1
#define PLAYERS_HOUSE_TV_MOVIE 2

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
