#ifndef GUARD_SLOT_MACHINE_H
#define GUARD_SLOT_MACHINE_H

void PlaySlotMachine(u8 machineId, MainCallback exitCallback);
void DestroyDigitalDisplayScene(void);
u8 CreateStdDigitalDisplaySprite(u8 templateIdx, u8 dispInfoId, s16 spriteId);
void Debug_PlaySlotMachine(u8 machineId, MainCallback exitCallback);

#endif // GUARD_SLOT_MACHINE_H
