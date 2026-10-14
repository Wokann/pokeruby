#ifndef GUARD_OVERWORLD_H
#define GUARD_OVERWORLD_H

#include "sprite.h"

struct InitialPlayerAvatarState
{
    u8 transitionFlags;
    u8 direction;
};

struct LinkPlayerObjectEvent
{
    u8 active;
    u8 linkPlayerId;
    u8 objEventId;
    u8 movementMode;
};

extern struct LinkPlayerObjectEvent gLinkPlayerObjectEvents[4];
extern const struct UCoords32 gDirectionToVectors[];
extern u16 gHeldKeyCodeToSend;
extern void (*gFieldCallback)(void);
extern u8 gFieldLinkPlayerCount;
extern u8 gLocalLinkPlayerId;

// sub_8052F5C
void Overworld_ResetStateAfterFly(void);
void Overworld_ResetStateAfterTeleport(void);
void Overworld_ResetStateAfterDigEscRope(void);
void ResetGameStats(void);
void IncrementGameStat(u8 index);
u32 GetGameStat(u8 index);
void SetGameStat(u8 index, u32 value);
// LoadObjEventTemplatesFromHeader
// LoadSaveblockObjEventScripts
void SetObjEventTemplateCoords(u8 localId, s16 x, s16 y);
void SetObjEventTemplateMovementType(u8 localId, u8 movementType);
// InitMapView
// ApplyCurrentWarp
// SetWarpData
// warp_data_is_not_neg_1
struct MapHeader const *const Overworld_GetMapHeaderByGroupAndId(u16 mapGroup, u16 mapNum);
struct MapHeader const *const GetDestinationWarpMapHeader(void);
// LoadSaveblockMapHeader
void WarpIntoMap(void);
void SetWarpDestination(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void SetWarpDestinationToMapWarp(s8 mapGroup, s8 mapNum, s8 warpId);
void SetDynamicWarp(s32 unused, s8 mapGroup, s8 mapNum, s8 warpId);
void SetDynamicWarpWithCoords(s32 unused, s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void SetWarpDestinationToDynamicWarp(u8 unusedWarpId);
void SetWarpDestinationToHealLocation(u8 healLocationId);
void SetWarpDestinationToLastHealLocation(void);
void SetLastHealLocationWarp(u8 healLocationId);
void UpdateEscapeWarp(s16 x, s16 y);
void SetEscapeWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void SetWarpDestinationToEscapeWarp(void);
void SetFixedDiveWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
// warp1_set_to_warp2
void SetFixedHoleWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void SetWarpDestinationToFixedHoleWarp(s16 x, s16 y);
void SetContinueGameWarpToHealLocation(u8 healLocationId);
void SetContinueGameWarpToDynamicWarp();
// GetMapConnection
bool8 SetDiveWarpEmerge(u16 x, u16 y);
bool8 SetDiveWarpDive(u16 x, u16 y);
void LoadMapFromCameraTransition(u8 mapGroup, u8 mapNum);
// LoadMapFromWarp
void ResetInitialPlayerAvatarState(void);
void StoreInitialPlayerAvatarState(void);
bool32 Overworld_IsBikingAllowed(void);
void SetDefaultFlashLevel(void);
void SetFlashLevel(s32 flashLevel);
u8 GetFlashLevel(void);
void SetCurrentMapLayout(u16 mapLayoutId);
// sub_8053D30
// sub_8053D6C
// GetLocationMusic
// GetCurrLocationDefaultMusic
// GetWarpDestinationMusic
void Overworld_ResetMapMusic(void);
void Overworld_PlaySpecialMapMusic(void);
void Overworld_SetSavedMusic(u16 songNum);
void Overworld_ClearSavedMusic(void);
void TransitionMapMusic(void);
void Overworld_ChangeMusicToDefault(void);
void Overworld_ChangeMusicTo(u16 newMusic);
// GetMapMusicFadeoutSpeed
void TryFadeOutOldMapMusic(void);
bool8 BGMusicStopped(void);
void Overworld_FadeOutMapMusic(void);
// PlayAmbientCry
void UpdateAmbientCry(s16 *state, u16 *delayCounter);
u8 GetMapTypeByGroupAndId(s8 mapGroup, s8 mapNum);
// GetMapTypeByWarpData
u8 GetCurrentMapType(void);
u8 GetLastUsedWarpMapType(void);
bool8 IsMapTypeOutdoors(u8 mapType);
bool8 Overworld_MapTypeAllowsTeleportAndFly(u8 mapType);
u8 IsMapTypeIndoors(u8 mapType);
u8 GetCurrentRegionMapSectionId();
// ResetSafariZoneFlag_
bool32 IsOverworldLinkActive(void);
// c1_overworld_normal
// c1_overworld
// OverworldBasic
void CB2_OverworldBasic(void);
// SetMainCallback1
// sub_80543DC
void CB2_NewGame(void);
void CB2_WhiteOut(void);
void CB2_LoadMap(void);
void CB2_ReturnToFieldContestHall(void);
void CB2_ReturnToFieldCableClub(void);
void CB2_ReturnToField(void);
// sub_805465C
void CB2_ReturnToFieldWithOpenMenu(void);
// CB2_ReturnToFieldContinueScript
void CB2_ReturnToFieldContinueScriptPlayMapMusic(void);
void CB2_ReturnToFieldFadeFromBlack(void);
// FieldCB_FadeTryShowMapPopup
void CB2_ContinueSavedGame(void);
bool32 LoadMapInStepsLocal(u8 *, bool32);
bool32 ReturnToFieldLocal(u8 *);
bool32 ReturnToFieldLink(u8 *state);
void DoMapLoadLoop(u8 *state);
void ResetScreenForMapLoad(void);
void InitViewGraphics(void);
void ResumeMap(bool32 a1);
void InitObjectEventsLocal(void);
void InitObjectEventsReturnToField(void);
// sub_8055218
// sub_8055280
// sub_80553E0
// sub_8055468
// sub_805546C
// sub_80554BC
s32 GetCableClubPartnersReady(void);
u16 SetInCableClubSeat(void);
u16 SetLinkWaitingForScript(void);
u16 QueueExitLinkRoomKey(void);
bool32 Overworld_IsRecvQueueAtMax(void);
bool32 Overworld_RecvKeysFromLinkIsRunning(void);
bool32 Overworld_SendKeysToLinkIsRunning(void);
bool32 IsSendingKeysOverCable(void);
void ClearLinkPlayerObjectEvents(void);
// ZeroObjectEvent
void CB2_ReturnToFieldFromMultiplayer(void);

void CB2_InitTestMenu(void);
void debug_sub_8058C00(void);
void CB2_ReturnToFieldContinueScript(void);

void SetContinueGameWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);

u8 GetSavedWarpRegionMapSectionId(void);

#endif // GUARD_OVERWORLD_H
