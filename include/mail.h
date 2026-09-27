#ifndef GUARD_MAIL_H
#define GUARD_MAIL_H

#include "main.h"

void ReadMail(struct MailStruct *mail, MainCallback exitCallback, bool8 hasText);

#endif // GUARD_MAIL_H
