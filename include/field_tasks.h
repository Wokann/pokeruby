#ifndef GUARD_FIELD_TASKS_H
#define GUARD_FIELD_TASKS_H

struct PacifidlogMetatileOffsets
{
    s8 x;
    s8 y;
    u16 metatileId;
};

void SetUpFieldTasks(void);
void ActivatePerStepCallback(u8 callbackId);
void ResetFieldTasksArgs(void);
void SetSootopolisGymCrackedIceMetatiles(void);

#endif // GUARD_FIELD_TASKS_H
