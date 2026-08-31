#ifndef GUARD_HEATED_ROCK_H
#define GUARD_HEATED_ROCK_H

// Eruption launch rock helpers.

u16 GetEruptionLaunchRockInitialYPos(u8 spriteId);
void InitEruptionLaunchRockCoordData(struct Sprite *sprite, s16 speedX, s16 speedY);

#endif // GUARD_HEATED_ROCK_H
