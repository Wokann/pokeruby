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

struct UCoords32
{
    u32 x, y;
};

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
// mapdata_load_assets_to_gpu_and_full_redraw
// ApplyCurrentWarp
// SetWarpData
// warp_data_is_not_neg_1
struct MapHeader * const Overworld_GetMapHeaderByGroupAndId(u16 mapGroup, u16 mapNum);
struct MapHeader * const GetDestinationWarpMapHeader(void);
// LoadSaveblockMapHeader
void WarpIntoMap(void);
void SetWarpDestination(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
void SetWarpDestinationToMapWarp(s8 mapGroup, s8 mapNum, s8 warpId);
void SetDynamicWarp(int unused, s8 mapGroup, s8 mapNum, s8 warpId);
void SetDynamicWarpWithCoords(int unused, s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);
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
// sub_8053994
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
void sub_8053F0C(void);
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
void sub_8054534(void);
void sub_8054588(void);
void CB2_ReturnToField(void);
// sub_805465C
void CB2_ReturnToFieldWithOpenMenu(void);
// CB2_ReturnToFieldContinueScript
void CB2_ReturnToFieldContinueScriptPlayMapMusic(void);
void CB2_ReturnToFieldFadeFromBlack(void);
// sub_805470C
void CB2_ContinueSavedGame(void);
bool32 sub_805493C(u8 *, u32);
bool32 sub_8054A4C(u8 *);
bool32 sub_8054A9C(u8 *a1);
void do_load_map_stuff_loop(u8 *state);
void sub_8054BA8(void);
void sub_8054C2C(void);
void sub_8054D4C(u32 a1);
void mli4_mapscripts_and_other(void);
void sub_8054E20(void);
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
u32 sub_80558AC(void);
u32 sub_8055910(void);
u32 sub_8055940(void);
void ClearLinkPlayerObjectEvents(void);
// ZeroObjectEvent
void CB2_ReturnToFieldFromMultiplayer(void);

void CB2_InitTestMenu(void);
void debug_sub_8058C00(void);
void CB2_ReturnToFieldContinueScript(void);

void SetContinueGameWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);

u8 GetSavedWarpRegionMapSectionId(void);

#endif // GUARD_OVERWORLD_H
