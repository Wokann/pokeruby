#ifndef GUARD_FIELD_TASKS_H
#define GUARD_FIELD_TASKS_H

struct PacifidlogMetatileOffsets {
    s8 x;
    s8 y;
    u16 metatileId;
};

void SetUpFieldTasks();
void ActivatePerStepCallback(u8);
void ResetFieldTasksArgs(void);

#endif // GUARD_FIELD_TASKS_H
