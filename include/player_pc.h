#ifndef GUARD_PLAYER_PC_H
#define GUARD_PLAYER_PC_H

struct PlayerPCItemPageStruct
{
 /*0x00*/ u8 cursorPos;
 /*0x01*/ u8 pageItems; // number of items (not including Cancel) on the current page.
 /*0x02*/ u8 itemsAbove; // number of items above the top item on the page.
 /*0x03*/ u8 count; // total number of items in mailbox storage.
};

void ReshowPlayerPC(u8 var);
void NewGameInitPCItems(void);
void BedroomPC(void);
void PlayerPC(void);
void ItemStorage_ReshowAfterBagMenu(void);
void Mailbox_ReturnToMailListAfterDeposit(void);

#endif // GUARD_PLAYER_PC_H
