#ifndef GUARD_DAYCARE_H
#define GUARD_DAYCARE_H

#include "constants/daycare.h"

struct RecordMixingDayCareMail
{
    struct DayCareMail mail[DAYCARE_MON_COUNT];
    u32 numDaycareMons;
    u16 cantHoldItem[DAYCARE_MON_COUNT]; // Empty slots and mons already holding an item cannot receive one.
};

u8 *GetMonNickname2(struct Pokemon *mon, u8 *dest);
u8 *GetBoxMonNickname(struct BoxPokemon *mon, u8 *dest);
u8 CountPokemonInDaycare(struct DayCare *daycare);
void InitDaycareMailRecordMixing(struct BoxPokemon *daycareMons, struct RecordMixingDayCareMail *mixMail);
void StoreSelectedPokemonInDaycare(void);
u16 TakePokemonFromDaycare(void);
void GetDaycareCostAndPrepareString(void);
u8 GetNumLevelsGainedFromDaycare(void);
void Debug_AddDaycareSteps(u16 numSteps);
u16 GetEggSpecies(u16 species);
void TriggerPendingDaycareEgg(void);
void TriggerPendingDaycareMaleEgg(void);
void RejectEggFromDayCare(void);
void CreateEgg(struct Pokemon *mon, u16 species, bool8 setHotSpringsLocation);
void GiveEggFromDaycare(void);
bool8 ShouldEggHatch(void);
u16 GetSelectedMonNicknameAndSpecies(void);
void GetDaycareMonNicknames(void);
u8 GetDaycareState(void);
u8 GetDaycareCompatibilityScoreFromSave(void);
void SetDaycareCompatibilityString(void);
bool8 NameHasGenderSymbol(const u8 *name, u8 genderRatio);
void ShowDaycareLevelMenu(void);
void ChooseSendDaycareMon(void);

#endif // GUARD_DAYCARE_H
