#ifndef GUARD_TV_H
#define GUARD_TV_H

#include "constants/tv.h"

extern u8 *const gTVStringVarPtrs[3];

void DoTVShow(void);
void DoTVShowInSearchOfTrainers(void);
void InterviewBefore(void);
void InterviewAfter(void);
void ClearTVShowData(void);
void StorePlayerIdInRecordMixShow(TVShow *show);
void StorePlayerIdInNormalShow(TVShow *show);
void BravoTrainerPokemonProfile_BeforeInterview1(u16 move);
void BravoTrainerPokemonProfile_BeforeInterview2(u8 contestStandingPlace);
void StartMassOutbreak(void);
void RecordFishingAttemptForTV(bool8 caughtFish);
void SetPokemonAnglerSpecies(u16 species);
bool8 IsPokeNewsActive(u8 newsKind);
size_t CountDigits(int value);
void CompactTVShowArray(TVShow shows[]);
u32 GetPlayerIDAsU32(void);
void ReceiveTvShowsData(u8 * src, u32 size, u8 playersLinkId);
void UpdateTVScreensOnMap(int width, int height);
void ResetGabbyAndTy(void);
u8 CheckForPlayersHouseNews(void);
bool8 IsGabbyAndTyShowOnTheAir(void);
u8 GabbyAndTyGetBattleNum(void);
void ReceivePokeNewsData(void * src, u32 size, u8 playersLinkId);
void TryPutSmartShopperOnAir(void);
void UpdateTVShowsPerDay(u16 days);
void DeactivateAllNormalTVShows(void);
void ConvertIntToDecimalString(u8 varIdx, s32 value);
void HideBattleTowerReporter(void);
void TryPutPokemonTodayOnAir(void);
u8 GetTVGroupByShowId(u8 kind);
void CompactPokeNews(void);

#endif // GUARD_TV_H
