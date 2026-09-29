#ifndef GUARD_FIELD_FADETRANSITION_H
#define GUARD_FIELD_FADETRANSITION_H

void FadeInFromBlack(void);
void FieldCB_ContinueScriptHandleMusic(void);
void FieldCB_ContinueScript(void);
void FieldCB_ReturnToFieldCableLink(void);
void FieldCB_ReturnToFieldWirelessLink(void);
void FieldCB_DefaultWarpExit(void);
void FieldCB_WarpExitFadeFromBlack(void);
void ReturnToFieldOpenStartMenu(void);
void FieldCB_ReturnToFieldNoScript(void);
void FieldCB_ReturnToFieldNoScriptCheckMusic(void);
void DoWarp(void);
void DoDiveWarp(void);
void DoDoorWarp(void);
void DoFallWarp(void);
void DoContestHallWarp(void);
void DoEscalatorWarp(u8);
void DoLavaridgeGymB1FWarp(void);
void DoLavaridgeGym1FWarp(void);
void DoTeleportTileWarp(void);
void DoPortholeWarp(void);
void DebugCycleWarpDestinationAndWarp(void);
void WarpFadeOutScreen(void);
void WarpFadeInScreen(void);

#endif // GUARD_FIELD_FADETRANSITION_H
