#ifndef GUARD_FIELD_PLAYER_AVATAR_H
#define GUARD_FIELD_PLAYER_AVATAR_H

enum {
    PLAYER_AVATAR_STATE_NORMAL,
    PLAYER_AVATAR_STATE_MACH_BIKE,
    PLAYER_AVATAR_STATE_ACRO_BIKE,
    PLAYER_AVATAR_STATE_SURFING,
    PLAYER_AVATAR_STATE_UNDERWATER,
    PLAYER_AVATAR_STATE_FIELD_MOVE,
    PLAYER_AVATAR_STATE_FISHING,
    PLAYER_AVATAR_STATE_WATERING,
};

void PlayerStep(u8 direction, u16 newKeys, u16 heldKeys);
u8 CheckForObjectEventCollision(struct ObjectEvent *objectEvent, s16 x, s16 y, u8 direction, u8 metatileBehavior);
void SetPlayerAvatarTransitionFlags(u16 transitionFlags);
void UpdatePlayerAvatarTransitionState(void);
u8 PlayerGetCopyableMovement(void);
void PlayerSetAnimId(u8 movementActionId, u8 copyableMovement);
void PlayerWalkNormal(u8 direction);
void PlayerWalkFast(u8 direction);
void PlayerRideWaterCurrent(u8 direction);
void PlayerWalkFaster(u8 direction);
void PlayerOnBikeCollide(u8 direction);
void PlayerFaceDirection(u8 direction);
void PlayerTurnInPlace(u8 direction);
void PlayerJumpLedge(u8 direction);
void PlayerFreeze(void);
void PlayerIdleWheelie(u8 direction);
void PlayerStartWheelie(u8 direction);
void PlayerEndWheelie(u8 direction);
void PlayerStandingHoppingWheelie(u8 direction);
void PlayerMovingHoppingWheelie(u8 direction);
void PlayerLedgeHoppingWheelie(u8 direction);
void PlayerAcroTurnJump(u8 direction);
void PlayerWheelieInPlace(u8 direction);
void PlayerPopWheelieWhileMoving(u8 direction);
void PlayerWheelieMove(u8 direction);
void PlayerEndWheelieWhileMoving(u8 direction);
void GetXYCoordsOneStepInFrontOfPlayer(s16 *x, s16 *y);
void PlayerGetDestCoords(s16 *x, s16 *y);
u8 GetPlayerFacingDirection(void);
u8 GetPlayerMovementDirection(void);
u8 PlayerGetElevation(void);
u8 TestPlayerAvatarFlags(u8 flag);
u8 GetPlayerAvatarFlags(void);
u8 GetPlayerAvatarSpriteId(void);
void CancelPlayerForcedMovement(void);
void StopPlayerAvatar(void);
u8 GetRivalAvatarGraphicsIdByStateIdAndGender(u8 state, u8 gender);
u8 GetPlayerAvatarGraphicsIdByStateId(u8 state);
u8 GetPlayerAvatarGenderByGraphicsId(u8 gfxId);
bool8 PartyHasMonWithSurf(void);
bool8 IsPlayerSurfingNorth(void);
bool8 IsPlayerFacingSurfableFishableWater(void);
void ClearPlayerAvatarInfo(void);
void SetPlayerAvatarStateMask(u8 flags);
u8 GetPlayerAvatarGraphicsIdByCurrentState(void);
void SetPlayerAvatarExtraStateTransition(u8 graphicsId, u8 transitionFlag);
void InitPlayerAvatar(s16 x, s16 y, u8 direction, u8 gender);
void SetPlayerInvisibility(bool8 invisible);
void SetPlayerAvatarFieldMove(void);
void PlayerUseAcroBikeOnBumpySlope(u8 direction);
void SetPlayerAvatarWatering(u8);
void StartFishing(u8 rod);

#endif
