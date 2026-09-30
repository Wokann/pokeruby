#ifndef GUARD_SAFARI_ZONE_H
#define GUARD_SAFARI_ZONE_H

extern u8 gNumSafariBalls;
// Used by Ruby's debug menu.
extern u16 gSafariZoneStepCounter;

bool32 GetSafariZoneFlag(void);
void SetSafariZoneFlag(void);
void ResetSafariZoneFlag(void);

void EnterSafariMode(void);
void ExitSafariMode(void);

bool8 SafariZoneTakeStep(void);
void SafariZoneRetirePrompt(void);
void CB2_EndSafariBattle(void);

struct Pokeblock *SafariZoneGetActivePokeblock(void);
void SafariZoneActivatePokeblockFeeder(u8 pkblId);

// Used by Ruby's debug menu.
bool32 GetInFrontFeederPokeblockAndSteps(void);

#endif // GUARD_SAFARI_ZONE_H
