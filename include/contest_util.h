#ifndef GUARD_CONTEST_UTIL_H
#define GUARD_CONTEST_UTIL_H

void SetBattleTowerPlayerParty(void);
void ReducePlayerPartyToSelectedMons(void);

u8 CountPlayerMuseumPaintings(void);
void ShowContestPainting(void);
void HealPlayerParty(void);
u8 ScriptGiveMon(u16 species, u8 level, u16 item, u32 unused1, u32 unused2, u8 unused3);
u8 ScriptGiveEgg(u16);
void CreateScriptedWildMon(u16, u8, u16);
void ScriptSetMonMoveSlot(u8, u16, u8);

#endif // GUARD_CONTEST_UTIL_H
