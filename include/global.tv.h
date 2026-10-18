#ifndef GUARD_GLOBAL_TV_H
#define GUARD_GLOBAL_TV_H

#include "constants/tv.h"

struct TVShowCommon
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 pad02[20];
    /*0x16*/ u16 var16[3];
    /*0x1C*/ u8 srcTrainerId3Lo;
    /*0x1D*/ u8 srcTrainerId3Hi;
    /*0x1E*/ u8 srcTrainerId2Lo;
    /*0x1F*/ u8 srcTrainerId2Hi;
    /*0x20*/ u8 srcTrainerIdLo;
    /*0x21*/ u8 srcTrainerIdHi;
    /*0x22*/ u8 trainerIdLo;
    /*0x23*/ u8 trainerIdHi;
};

struct TVShowFanClubLetter
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u16 species;
    /*0x04*/ u16 words[6];
    /*0x10*/ u8 playerName[8];
    /*0x18*/ u8 language;
};

struct TVShowRecentHappenings
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u16 species;
    /*0x04*/ u16 words[6];
    /*0x10*/ u8 playerName[8];
    /*0x18*/ u8 language;
    /*0x19*/ u8 pad19[10];
};

struct TVShowFanclubOpinions
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u16 species;
    /*0x04*/ u8 friendshipHighNybble:4;
    /*0x04*/ u8 questionAsked:4;
    /*0x05*/ u8 playerName[8];
    /*0x0D*/ u8 language;
    /*0x0E*/ u8 pokemonNameLanguage;
    /*0x0F*/ u8 filler_0F;
    /*0x10*/ u8 nickname[8];
    /*0x18*/ u16 words18[2];
    /*0x1C*/ u16 words[4];
};

struct TVShowUnknownType04
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 pad02[4];
    /*0x06*/ u16 species;
};

struct TVShowNameRaterShow
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u16 species;
    /*0x04*/ u8 pokemonName[11];
    /*0x0F*/ u8 trainerName[11];
    /*0x1A*/ u8 random;
    /*0x1B*/ u8 random2;
    /*0x1C*/ u16 randomSpecies;
    /*0x1E*/ u8 language;
    /*0x1F*/ u8 pokemonNameLanguage;
};

struct TVShowBravoTrainerPokemonProfiles
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u16 species;
    /*0x04*/ u16 words[2];
    /*0x08*/ u8 pokemonNickname[11];
    /*0x13*/ u8 contestCategory:3;
    /*0x13*/ u8 contestRank:2;
    /*0x13*/ u8 contestResult:2;
    /*0x13*/ u8 var13_7:1;
    /*0x14*/ u16 move;
    /*0x16*/ u8 playerName[8];
    /*0x1E*/ u8 language;
    /*0x1F*/ u8 pokemonNameLanguage;
};

struct TVShowBravoTrainerBattleTowerSpotlight
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 playerName[8];
    /*0x0A*/ u16 species;
    /*0x0C*/ u8 opponentName[8];
    /*0x14*/ u16 defeatedSpecies;
    /*0x16*/ u16 numFights;
    /*0x18*/ u16 words[1];
    /*0x1A*/ u8 btLevel;
    /*0x1B*/ u8 interviewResponse;
    /*0x1C*/ u8 wonTheChallenge;
    /*0x1D*/ u8 playerLanguage;
};

struct TVShowPokemonToday
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 language;
    /*0x03*/ u8 language2;
    /*0x04*/ u8 nickname[11];
    /*0x0F*/ u8 ball;
    /*0x10*/ u16 species;
    /*0x12*/ u8 nBallsUsed;
    /*0x13*/ u8 playerName[8];
};

struct TVShowSmartShopper
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 priceReduced;
    /*0x03*/ u8 language;
    /*0x04*/ u8 pad04[2];
    /*0x06*/ u16 itemIds[3];
    /*0x0C*/ u16 itemAmounts[3];
    /*0x12*/ u8 shopLocation;
    /*0x13*/ u8 playerName[8];
};

struct TVShowPokemonTodayFailed
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 language;
    /*0x03*/ u8 pad03[9];
    /*0x0c*/ u16 species;
    /*0x0e*/ u16 species2;
    /*0x10*/ u8 nBallsUsed;
    /*0x11*/ u8 outcome;
    /*0x12*/ u8 location;
    /*0x13*/ u8 playerName[8];
};

struct TVShowPokemonAngler
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 nBites;
    /*0x03*/ u8 nFails;
    /*0x04*/ u16 species;
    /*0x06*/ u8 language;
             u8 pad07[12];
    /*0x13*/ u8 playerName[8];
};

struct TVShowWorldOfMasters
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u16 numPokeCaught;
    /*0x04*/ u16 caughtPoke;
    /*0x06*/ u16 steps;
    /*0x08*/ u16 species;
    /*0x0a*/ u8 location;
    /*0x0b*/ u8 language;
             u8 pad0c[7];
    /*0x13*/ u8 playerName[8];
};

struct TVShowMassOutbreak
{
    /*0x00*/ u8 kind;
    /*0x01*/ bool8 active;
    /*0x02*/ u8 unused1;
    /*0x03*/ u8 unused3;
    /*0x04*/ u16 moves[4];
    /*0x0C*/ u16 species;
    /*0x0E*/ u16 unused2;
    /*0x10*/ u8 locationMapNum;
    /*0x11*/ u8 locationMapGroup;
    /*0x12*/ u8 unused4;
    /*0x13*/ u8 probability;
    /*0x14*/ u8 level;
    /*0x15*/ u8 unused5;
    /*0x16*/ u16 daysBeforeOutbreak;
    /*0x18*/ u8 language;
             u8 pad19[11];
};

typedef union TVShow
{
    struct TVShowCommon common;
    struct TVShowFanClubLetter fanclubLetter;
    struct TVShowRecentHappenings recentHappenings;
    struct TVShowFanclubOpinions fanclubOpinions;
    struct TVShowUnknownType04 dummy;
    struct TVShowNameRaterShow nameRaterShow;
    struct TVShowBravoTrainerPokemonProfiles bravoTrainer;
    struct TVShowBravoTrainerBattleTowerSpotlight bravoTrainerTower;
    struct TVShowPokemonToday pokemonToday;
    struct TVShowSmartShopper smartshopperShow;
    struct TVShowPokemonTodayFailed pokemonTodayFailed;
    struct TVShowPokemonAngler pokemonAngler;
    struct TVShowWorldOfMasters worldOfMasters;
    struct TVShowMassOutbreak massOutbreak;
} TVShow;

struct PokeNews
{
    u8 kind;
    u8 state;
    u16 dayCountdown;
};

struct GabbyAndTyData
{
    /*2b10*/ u16 mon1;
    /*2b12*/ u16 mon2;
    /*2b14*/ u16 lastMove;
    /*2b16*/ u16 quote;
    /*2b18*/ u8 mapnum;
    /*2b19*/ u8 battleNum;
    /*2b1a*/ u8 battleTookMoreThanOneTurn:1;
    /*2b1a*/ u8 playerLostAMon:1;
    /*2b1a*/ u8 playerUsedHealingItem:1;
    /*2b1a*/ u8 playerThrewABall:1;
    /*2b1a*/ u8 onAir:1;
    /*2b1a*/ u8 valA_5:3;
    /*2b1b*/ u8 battleTookMoreThanOneTurn2:1;
    /*2b1b*/ u8 playerLostAMon2:1;
    /*2b1b*/ u8 playerUsedHealingItem2:1;
    /*2b1b*/ u8 playerThrewABall2:1;
    /*2b1b*/ u8 valB_4:1;
    /*2b1b*/ u8 valB_5:3;
};

#endif // GUARD_GLOBAL_TV_H
