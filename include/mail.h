#ifndef GUARD_MAIL_H
#define GUARD_MAIL_H

#include "main.h"

#define IS_ITEM_MAIL(itemId) ((itemId) >= ITEM_ORANGE_MAIL && (itemId) <= ITEM_RETRO_MAIL)

// mail.h
void HandleReadMail(struct Mail *arg0, MainCallback arg1, bool8 arg2);

// mail_data.h
void ClearAllMail(void);
void ClearMail(struct Mail *mail);
bool8 MonHasMail(struct Pokemon *mon);
u8 GiveMailToMonByItemId(struct Pokemon *mon, u16 itemId);
u16 SpeciesToMailSpecies(u16 species, u32 personality);
u16 MailSpeciesToSpecies(u16 mailSpecies, u16 *buffer);
u8 GiveMailToMon(struct Pokemon *mon, struct Mail *mail);
void TakeMailFromMon(struct Pokemon *mon);
void ClearMailItemId(u8 mailId);
u8 TakeMailFromMonAndSave(struct Pokemon *mon);
bool8 ItemIsMail(u16 itemId);

#endif // GUARD_MAIL_H
