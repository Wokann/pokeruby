#ifndef GUARD_SCRIPT_PARTY_MENU_H
#define GUARD_SCRIPT_PARTY_MENU_H

struct UnknownStruct2018000
{
    u8 filler0[0x8];
    u8 unk8;
};

void OpenPartyMenuFromScriptContext(u8 taskId);
void DrawContestPartyMonStatus(void);
void DrawMoveRelearnerPartyMonStatus(void);
void ChooseContestMon(void);
void HandleSelectPartyMenu(u8 taskId);
bool8 SetupContestPartyMenu(void);
void HandleMoveRelearnerPartyMenu(u8 taskId);
bool8 SetupMoveRelearnerPartyMenu(void);

#endif
