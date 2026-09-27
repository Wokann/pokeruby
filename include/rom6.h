#ifndef GUARD_ROM6_H
#define GUARD_ROM6_H

extern struct MapPosition gPlayerFacingPosition;

bool8 CheckObjectGraphicsInFrontOfPlayer(u8 graphicsId);
u8 CreateFieldMoveTask(void);
void Debug_UseRockSmashInFrontOfPlayer(void);

#endif
