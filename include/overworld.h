#ifndef GUARD_ROM4_H
#define GUARD_ROM4_H

#include "sprite.h"

#define LINK_KEY_CODE_EMPTY 0x11
#define LINK_KEY_CODE_DPAD_DOWN 0x12
#define LINK_KEY_CODE_DPAD_UP 0x13
#define LINK_KEY_CODE_DPAD_LEFT 0x14
#define LINK_KEY_CODE_DPAD_RIGHT 0x15
#define LINK_KEY_CODE_READY 0x16
#define LINK_KEY_CODE_EXIT_ROOM 0x17
#define LINK_KEY_CODE_START_BUTTON 0x18
#define LINK_KEY_CODE_A_BUTTON 0x19
#define LINK_KEY_CODE_IDLE 0x1A
#define LINK_KEY_CODE_HANDLE_RECV_QUEUE 0x1B
#define LINK_KEY_CODE_HANDLE_SEND_QUEUE 0x1C
#define LINK_KEY_CODE_EXIT_SEAT 0x1D
#define LINK_KEY_CODE_UNK_8 0x1E

#define MOVEMENT_MODE_FREE 0
#define MOVEMENT_MODE_FROZEN 1
#define MOVEMENT_MODE_SCRIPTED 2

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

struct UCoords32
{
    u32 x, y;
};

extern const struct UCoords32 gDirectionToVectors[];
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
void SetGameStat(u8, u32);
// LoadObjEventTemplatesFromHeader
// LoadSaveblockObjEventScripts
void Overworld_SetObjEventTemplateCoords(u8, s16, s16);
void Overworld_SetObjEventTemplateMovementType(u8, u8);
// mapdata_load_assets_to_gpu_and_full_redraw
// ApplyCurrentWarp
// SetWarpData
// warp_data_is_not_neg_1
struct MapHeader * const Overworld_GetMapHeaderByGroupAndId(u16 mapGroup, u16 mapNum);
struct MapHeader * const GetDestinationWarpMapHeader(void);
// LoadSaveblockMapHeader
void WarpIntoMap(void);
void Overworld_SetWarpDestination(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void warp1_set_2(s8 mapGroup, s8 mapNum, s8 warpId);
void saved_warp2_set(int unused, s8 mapGroup, s8 mapNum, s8 warpId);
void saved_warp2_set_2(int unused, s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void SetWarpDestinationToDynamicWarp(u8 unusedWarpId);
void SetWarpDestinationToHealLocation(u8 healLocationId);
void Overworld_SetWarpDestToLastHealLoc(void);
void Overworld_SetHealLocationWarp(u8);
void UpdateEscapeWarp(s16 x, s16 y);
void SetEscapeWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void SetWarpDestinationToEscapeWarp(void);
void SetFixedDiveWarp(s8, s8, s8, s8, s8);
// warp1_set_to_warp2
void SetFixedHoleWarp(s8, s8, s8, s8, s8);
void SetFixedHoleWarpAsDestination(s16, s16);
void SetContinueGameWarpToHealLocation(u8);
void gpu_sync_bg_hide();
// GetMapConnection
bool8 SetDiveWarpEmerge(u16 x, u16 y);
bool8 SetDiveWarpDive(u16 x, u16 y);
void LoadMapFromCameraTransition(u8 mapGroup, u8 mapNum);
void ResetInitialPlayerAvatarState(void);
void StoreInitialPlayerAvatarState(void);
u16 GetCenterScreenMetatileBehavior(void);
bool32 Overworld_IsBikingAllowed(void);
void SetDefaultFlashLevel(void);
void Overworld_SetFlashLevel(s32 a1);
u8 Overworld_GetFlashLevel(void);
void SetCurrentMapLayout(u16 mapLayoutId);
// sub_8053D30
// sub_8053D6C
// GetLocationMusic
// GetCurrLocationDefaultMusic
// GetWarpDestinationMusic
void Overworld_ResetMapMusic(void);
void Overworld_PlaySpecialMapMusic(void);
void Overworld_SetSavedMusic(u16);
void Overworld_ClearSavedMusic(void);
void TransitionMapMusic(void);
void Overworld_ChangeMusicToDefault(void);
void Overworld_ChangeMusicTo(u16);
// GetMapMusicFadeoutSpeed
void TryFadeOutOldMapMusic(void);
bool8 BGMusicStopped(void);
void Overworld_FadeOutMapMusic(void);
// PlayAmbientCry
void UpdateAmbientCry(s16 *, u16 *);
u8 GetMapTypeByGroupAndId(s8 mapGroup, s8 mapNum);
// GetMapTypeByWarpData
u8 Overworld_GetMapTypeOfSaveblockLocation(void);
u8 GetLastUsedWarpMapType(void);
bool8 IsMapTypeOutdoors(u8 a1);
bool8 Overworld_MapTypeAllowsTeleportAndFly(u8 a1);
u8 Overworld_MapTypeIsIndoors(u8);
u8 GetSavedWarpRegionMapSectionId(void);
u8 GetCurrentRegionMapSectionId(void);
u8 GetCurrentMapBattleScene(void);
// ResetSafariZoneFlag_
bool32 IsOverworldLinkActive(void);
// c1_overworld_normal
// c1_overworld
// OverworldBasic
void CB2_OverworldBasic(void);
// SetMainCallback1
// SetKeyInterceptCallback
void CB2_NewGame(void);
void CB2_WhiteOut(void);
void CB2_LoadMap(void);
void CB2_LoadMap2(void);
void CB2_ReturnToFieldContestHall(void);
void CB2_ReturnToFieldCableClub(void);
void CB2_ReturnToField(void);
void CB2_ReturnToFieldLocal(void);
void CB2_ReturnToFieldLink(void);
void CB2_ReturnToFieldWithOpenMenu(void);
void CB2_ReturnToFieldContinueScript(void);
void CB2_ReturnToFieldContinueScriptPlayMapMusic(void);
void CB2_ReturnToFieldFadeFromBlack(void);
void CB2_ContinueSavedGame(void);
void FieldClearVBlankHBlankCallbacks(void);
bool32 LoadMapInStepsLocal(u8 *, u32);
bool32 ReturnToFieldLocal(u8 *);
bool32 ReturnToFieldLink(u8 *a1);
void DoMapLoadLoop(u8 *a1);
void ResetScreenForMapLoad(void);
void InitViewGraphics(void);
void ResumeMap(u32 a1);
void InitObjectEventsLink(void);
void InitObjectEventsLocal(void);
void InitObjectEventsReturnToField(void);
void SetCameraToTrackPlayer(void);
void SetCameraToTrackGuestPlayer(void);
void SetCameraToTrackGuestPlayer_2(void);
void OffsetCameraFocusByLinkPlayerId(void);
void SpawnLinkPlayers(void);
void CreateLinkPlayerSprites(void);
void ResetAllPlayerLinkStates(void);
// AreAllPlayersInLinkState
// IsAnyPlayerInLinkState
// HandleLinkPlayerKeyInput
// UpdateAllLinkPlayers
// UpdateHeldKeyCode
// KeyInterCB_ReadButtons
u16 GetDirectionForDpadKey(u16);
void ResetPlayerHeldKeys(u16 *);
void CB1_OverworldLink(void);
u16 KeyInterCB_SelfIdle(u32);
// KeyInterCB_Idle
u16 KeyInterCB_DeferToEventScript(u32);
u16 KeyInterCB_DeferToRecvQueue(u32);
u16 KeyInterCB_DeferToSendQueue(u32);
// KeyInterCB_ExitingSeat
// KeyInterCB_Ready
// KeyInterCB_SetReady
// KeyInterCB_SendNothing
// KeyInterCB_WaitForPlayersToExit
// KeyInterCB_SendExitRoomKey
s32 GetCableClubPartnersReady(void);
u16 SetInCableClubSeat(void);
u16 SetLinkWaitingForScript(void);
u16 QueueExitLinkRoomKey(void);
void LoadCableClubPlayer(int linkPlayerId, int selfId, struct CableClubPlayer *trainer);
bool32 IsCableClubPlayerUnfrozen(struct CableClubPlayer *);
bool32 CanCableClubPlayerPressStart(struct CableClubPlayer *);
const u8 *TryGetTileEventScript(struct CableClubPlayer *);
bool32 PlayerIsAtSouthExit(struct CableClubPlayer *);
const u8 *TryInteractWithPlayer(struct CableClubPlayer *);
void InitLinkPlayerQueueScript(void);
void InitLinkRoomStartMenuScript(void);
void RunConfirmLeaveCableClubScript(void);
void RunTerminateLinkScript(void);
bool32 Overworld_IsRecvQueueAtMax(void);
u32 Overworld_RecvKeysFromLinkIsRunning(void);
u32 Overworld_SendKeysToLinkIsRunning(void);
u32 IsSendingKeysOverCable(void);
// ZeroLinkPlayerObjectEvent
void ClearLinkPlayerObjectEvents(void);
// ZeroObjectEvent
// SetLinkPlayerObjectRange
// DestroyLinkPlayerObject
u8 GetSpriteForLinkedPlayer(u8);
void GetLinkPlayerCoords(u8, u16 *, u16 *);
u8 GetLinkPlayerFacingDirection(u8);
u8 GetLinkPlayerElevation(u8);
// GetLinkPlayerObjectStepTimer
void SetPlayerFacingDirection(u8, u8);
// MovementEventModeCB_Normal
// MovementEventModeCB_Ignored
// MovementEventModeCB_Scripted
// FacingHandler_DoNothing
// FacingHandler_DpadMovement
// FacingHandler_ForcedFacingChange
// MovementStatusHandler_EnterFreeMode
// MovementStatusHandler_TryAdvanceScript
void CB2_ReturnToFieldFromMultiplayer(void);

void CB2_InitTestMenu(void);
void debug_sub_8058C00(void);

#endif // GUARD_ROM4_H
