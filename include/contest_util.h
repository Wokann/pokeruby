#ifndef GUARD_CONTEST_UTIL_H
#define GUARD_CONTEST_UTIL_H

void SetBattleTowerPlayerParty(void);
void ReducePlayerPartyToThree(void);

u8 CountPlayerMuseumPaintings(void);
void ShowContestWinner(void);
void HealPlayerParty(void);
u8 ScriptGiveMon(u16, u8, u16, u32, u32, u8);
u8 ScriptGiveEgg(u16);
void CreateScriptedWildMon(u16, u8, u16);
void ScriptSetMonMoveSlot(u8, u16, u8);

#endif // GUARD_CONTEST_UTIL_H
