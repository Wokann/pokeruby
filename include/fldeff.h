#ifndef GUARD_FLDEFF_H
#define GUARD_FLDEFF_H

extern struct MapPosition gPlayerFacingPosition;

// teleport
bool8 SetUpFieldMove_Teleport(void);
bool8 FldEff_UseTeleport(void);

// dig
bool8 SetUpFieldMove_Dig(void);
bool8 FldEff_UseDig(void);

// rock smash
bool8 CheckObjectGraphicsInFrontOfPlayer(u8 graphicsId);
u8 CreateFieldMoveTask(void);
bool8 SetUpFieldMove_RockSmash(void);
bool8 FldEff_UseRockSmash(void);

void Debug_UseRockSmashInFrontOfPlayer(void);

#endif // GUARD_FLDEFF_H
