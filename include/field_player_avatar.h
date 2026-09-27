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

// MovementType_None2
void player_step(u8 a, u16 b, u16 c);
u8 CheckForObjectEventCollision(struct ObjectEvent *a, s16 b, s16 c, u8 d, u8 e);
void SetPlayerAvatarTransitionFlags(u16 transitionFlags);
void UpdatePlayerAvatarTransitionState(void);
u8 PlayerGetCopyableMovement(void);
void PlayerSetAnimId(u8 a, u8 b);
void PlayerWalkNormal(u8 a);
void PlayerWalkFast(u8 a);
void PlayerRideWaterCurrent(u8 a);
void PlayerWalkFaster(u8 a);
void PlayerOnBikeCollide(u8);
void PlayerFaceDirection(u8 a);
void PlayerTurnInPlace(u8 a);
void PlayerJumpLedge(u8 a);
void PlayerFreeze(void);
void PlayerIdleWheelie(u8 a);
void PlayerStartWheelie(u8 a);
void PlayerEndWheelie(u8 a);
void PlayerStandingHoppingWheelie(u8 a);
void PlayerMovingHoppingWheelie(u8 a);
void PlayerLedgeHoppingWheelie(u8 a);
void PlayerAcroTurnJump(u8 a);
void PlayerWheelieInPlace(u8 a);
void PlayerPopWheelieWhileMoving(u8 a);
void PlayerWheelieMove(u8 a);
void PlayerEndWheelieWhileMoving(u8 a);
void GetXYCoordsOneStepInFrontOfPlayer(s16 *x, s16 *y);
void PlayerGetDestCoords(s16 *, s16 *);
u8 GetPlayerFacingDirection(void);
u8 GetPlayerMovementDirection(void);
u8 PlayerGetElevation(void);
u8 TestPlayerAvatarFlags(u8);
u8 GetPlayerAvatarFlags(void);
u8 GetPlayerAvatarSpriteId(void);
void CancelPlayerForcedMovement(void);
void StopPlayerAvatar(void);
u8 GetRivalAvatarGraphicsIdByStateIdAndGender(u8 a, u8 b);
u8 GetPlayerAvatarGraphicsIdByStateId(u8);
u8 GetPlayerAvatarGenderByGraphicsId(u8 gfxId);
bool8 PartyHasMonWithSurf(void);
bool8 IsPlayerSurfingNorth(void);
bool8 IsPlayerFacingSurfableFishableWater(void);
void ClearPlayerAvatarInfo(void);
void SetPlayerAvatarStateMask(u8 a);
u8 GetPlayerAvatarGraphicsIdByCurrentState(void);
void SetPlayerAvatarExtraStateTransition(u8 a, u8 b);
void InitPlayerAvatar(s16 a, s16 b, u8 c, u8 d);
void SetPlayerInvisibility(bool8 invisible);
void SetPlayerAvatarFieldMove(void);
void PlayerUseAcroBikeOnBumpySlope(u8);
void SetPlayerAvatarWatering(u8);
void StartFishing(u8 rod);

#endif
