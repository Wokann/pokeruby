#ifndef GUARD_TV_H
#define GUARD_TV_H

#include "constants/tv.h"

extern u8 *const gTVStringVarPtrs[3];

void ClearTVShowData(void);
void InitWorldOfMastersShowAttempt(void);
void TryPutPokemonTodayFailedOnTheAir(void);
void StorePlayerIdInRecordMixShow(TVShow *show);
void StorePlayerIdInNormalShow(TVShow *show);
void BravoTrainerPokemonProfile_BeforeInterview1(u16 move);
void BravoTrainerPokemonProfile_BeforeInterview2(u8 contestStandingPlace);
void StartMassOutbreak(void);
void TryStartRandomMassOutbreak(void);
void UpdateTimeBeforeMassOutbreak(u16 days);
void TryEndMassOutbreak(u16 days);
void RecordFishingAttemptForTV(bool8 caughtFish);
void TryPutFishingAdviceOnAir(void);
void SetPokemonAnglerSpecies(u16);
void ResolveWorldOfMastersShow(u16 days);
void TryPutWorldOfMastersOnAir(void);
void TryPutRandomPokeNewsOnAir(void);
int GetFirstEmptyPokeNewsSlot(struct PokeNews *pokeNews);
void ClearPokeNews(void);
void ClearPokeNewsBySlot(u8 slot);
bool8 IsPokeNewsActive(u8 newsKind);
bool8 ShouldApplyPokeNewsEffect(u8 newsKind);
bool8 IsAddingPokeNewsDisallowed(u8 newsKind);
void UpdatePokeNewsCountdown(u16 days);
void CopyContestRankToStringVar(u8 varIdx, u8 rank);
void ConvertIntToDecimalString(u8 varIdx, s32 value);
size_t CountDigits(int value);
bool8 IsRecordMixShowAlreadySpawned(u8 showKind);
void SortPurchasesByQuantity(void);
void DeleteTVShowInArrayByIdx(TVShow tvShows[], u8 showIdx);
void CompactTVShowArray(TVShow tvShows[]);
u16 GetRandomDifferentSpeciesAndNameSeenByPlayer(u8 varIdx, u16 excludedSpecies);
u16 GetRandomDifferentSpeciesSeenByPlayer(u16 excludedSpecies);
void Script_FindFirstEmptyNormalTVShowSlot(void);
s8 FindFirstEmptyNormalTVShowSlot(TVShow *tvShows);
s8 FindFirstEmptyRecordMixTVShowSlot(TVShow tvShows[]);
bool8 BernoulliTrial(u16 ratio);
u32 GetPlayerIDAsU32(void);
void ReceiveTvShowsData(u8 *src, u32 size, u8 playersLinkId);
u8 TryMixNormalTVShow(TVShow *dest, TVShow *src, u8 sourcePlayerIdx);
u8 TryMixRecordMixTVShow(TVShow *dest, TVShow *src, u8 sourcePlayerIdx);
u8 TryMixOutbreakTVShow(TVShow *dest, TVShow *src, u8 sourcePlayerIdx);
s8 FindInactiveShowInArray(TVShow tvShows[]);
void DeactivateShowsWithUnseenSpecies(void);
void DeactivateShow(u8 showIdx);
void DeactivateShowIfNotSeenSpecies(u16 species, u8 showIdx);
void DeactivateGameCompleteShowsIfNotUnlocked(void);
bool8 TryMixPokeNewsShow(struct PokeNews *dest, struct PokeNews *src, s8 slot);
void TVShowConvertInternationalString(u8 *dest, u8 *src, u8 language);
void DoTVShowTheNameRaterShow(void);
void DoTVShowPokemonTodaySuccessfulCapture(void);
void DoTVShowPokemonTodayFailedCapture(void);
void DoTVShowPokemonFanClubLetter(void);
void DoTVShowRecentHappenings(void);
void DoTVShowPokemonFanClubOpinions(void);
void nullsub_22(void);
void DoTVShowPokemonNewsMassOutbreak(void);
void DoTVShowPokemonAngler(void);
void DoTVShowTheWorldOfMasters(void);
void TVShowDone(void);
void UpdateTVScreensOnMap(int, int);
void ResetGabbyAndTy(void);
void TakeGabbyAndTyOffTheAir(void);
extern u8 GabbyAndTyGetBattleNum(void);
void DeleteExcessMixedShows(void);
void ReceivePokeNewsData(void *, u32, u8);
void DoTVShowBravoTrainerPokemonProfile(void);
void DoTVShowBravoTrainerBattleTower(void);
void DoTVShowTodaysSmartShopper(void);
void TryPutSmartShopperOnAir(void);
void UpdateTVShowsPerDay(u16);
void DeactivateAllNormalTVShows(void);
void HideBattleTowerReporter(void);
void TryPutPokemonTodayOnAir(void);
u8 GetTVGroupByShowId(u8);
void CompactPokeNews(void);

#endif // GUARD_TV_H
