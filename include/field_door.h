#ifndef GUARD_FIELD_DOOR_H
#define GUARD_FIELD_DOOR_H

struct DoorGraphics
{
    u16 metatileNum;
    u8 sound;
    const void *tiles;
    const void *palettes;
};

struct DoorAnimFrame
{
    u8 time;
    u16 offset;
};

void FieldSetDoorOpened(u32 x, u32 y);
void FieldSetDoorClosed(u32 x, u32 y);
s8 FieldAnimateDoorClose(u32 x, u32 y);
s8 FieldAnimateDoorOpen(u32 x, u32 y);
bool8 FieldIsDoorAnimationRunning(void);
u32 GetDoorSoundEffect(u32 x, u32 y);

#endif
