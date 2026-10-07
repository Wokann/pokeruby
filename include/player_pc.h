#ifndef GUARD_PLAYER_PC_H
#define GUARD_PLAYER_PC_H

// local task defines
#define PAGE_INDEX data[0]
#define ITEMS_ABOVE_TOP data[1]
#define NUM_PAGE_ITEMS data[4]
// not used
#define CURRENT_ITEM_STORAGE_MENU data[6]
// not used
#define SWAP_ITEM_INDEX data[8]
#define SWITCH_MODE_ACTIVE data[9]

// mailbox mail options
enum
{
    MAILBOX_READ,
    MAILBOX_MOVE_TO_BAG,
    MAILBOX_GIVE,
    MAILBOX_CANCEL,
};

struct MailboxStruct
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
void ItemStorage_ReturnToMenuAfterDeposit(void);
void Mailbox_ReturnToMailListAfterDeposit(void);

#endif // GUARD_PLAYER_PC_H
