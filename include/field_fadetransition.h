#ifndef GUARD_FIELD_FADETRANSITION_H
#define GUARD_FIELD_FADETRANSITION_H

void FillPalBufferWhite(void);
void FillPalBufferBlack(void);
void FadeInFromBlack(void);
void FadeInFromBlack();
void FadeInFromBlack(void);
void Task_WaitForFadeAndEnableScriptCtx(u8);
void FieldCB_ContinueScriptHandleMusic(void);
void FieldCB_ContinueScript(void);
void FieldCB_ReturnToFieldCableLink(void);
void FieldCB_ReturnToFieldWirelessLink(void);
void mapldr_default();
void FieldCB_WarpExitFadeFromBlack(void);
void sub_8080DEC(void);
void sub_8080E28(void);
void FieldCB_ReturnToFieldNoScriptCheckMusic(void);
bool32 sub_8080E70(void);
void sub_8080E88(void);
void DoDiveWarp(void);
void sub_8080EF0(void);
void DoFallWarp(void);
void DoContestHallWarp(void);
void sub_8080F2C(u8);
void sub_8080F48(void);
void sub_8080F58(void);
void sub_8080F68(void);
void DoPortholeWarp(void);
void debug_sub_80888D8(void);
void WarpFadeOutScreen(void);
void WarpFadeInScreen(void);

#endif // GUARD_FIELD_FADETRANSITION_H
