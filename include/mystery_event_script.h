#ifndef GUARD_MYSTERY_EVENT_SCRIPT_H
#define GUARD_MYSTERY_EVENT_SCRIPT_H

enum {
    MEVENT_STATUS_LOAD_OK,
    MEVENT_STATUS_LOAD_ERROR,
    MEVENT_STATUS_SUCCESS,
    MEVENT_STATUS_FAILURE,
    MEVENT_STATUS_FF = 0xFF
};

u32 RunMysteryEventScript(u8 *script);
void SetMysteryEventScriptStatus(u32 status);
u16 GetRecordMixingGift(void);
#if DEBUG
bool8 TrySetMysteryEventScriptCRC(u8 *script, const u8 *sourceScript);
bool8 TrySetMysteryEventScriptChecksum(u8 *script, const u8 *sourceScript);
#endif // DEBUG

#endif // GUARD_MYSTERY_EVENT_SCRIPT_H
