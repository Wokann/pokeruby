#ifndef GUARD_FIELD_MESSAGE_BOX_H
#define GUARD_FIELD_MESSAGE_BOX_H

enum
{
    FIELD_MESSAGE_BOX_HIDDEN,
    FIELD_MESSAGE_BOX_UNUSED,
    FIELD_MESSAGE_BOX_NORMAL,
    FIELD_MESSAGE_BOX_AUTO_SCROLL,
};

void InitFieldMessageBox(void);
bool8 ShowFieldMessage(const u8 *message);
bool8 ShowFieldAutoScrollMessage(const u8 *message);
bool8 ForceShowFieldAutoScrollMessage(const u8 *message);
bool8 ShowFieldMessageFromBuffer(const u8 *message);
void HideFieldMessageBox(void);
u8 GetFieldMessageBoxMode(void);
bool8 IsFieldMessageBoxHidden(void);
void unref_sub_8064CA0(void);

#endif // GUARD_FIELD_MESSAGE_BOX_H
