#ifndef GUARD_SECRET_BASE_H
#define GUARD_SECRET_BASE_H

void ClearSecretBases(void);
void TrySetCurSecretBaseIndex(void);
void CheckPlayerHasSecretBase(void);
void ToggleSecretBaseEntranceMetatile(void);
void SetOccupiedSecretBaseEntranceMetatiles(const struct MapEvents *events);
bool8 SecretBaseMapPopupEnabled(void);
bool8 CurMapIsSecretBase(void);
void InitSecretBaseAppearance(bool8 hidePC);
void SetCurSecretBaseIdFromPosition(const struct MapPosition *position, const struct MapEvents *events);
void WarpIntoSecretBase(const struct MapPosition *position, const struct MapEvents *events);
bool8 TrySetCurSecretBase(void);
u8 *GetSecretBaseMapName(u8 *dest);
void CopyCurSecretBaseOwnerName_StrVar1(void);
void SetPlayerSecretBaseParty(void);
const u8 *GetSecretBaseTrainerLoseText(void);
void SecretBasePerStepCallback(u8 taskId);
void ReceiveSecretBasesData(void *secretBases, u32 recordSize, u8 linkIdx);
#if DEBUG
void unref_sub_80BCD7C(u8 secretBaseIndex);
u8 *GetSecretBaseName(u8 *dest, u8 secretBaseIdx);
#endif

#endif // GUARD_SECRET_BASE_H
