#ifndef GUARD_TRAINER_SEE_H
#define GUARD_TRAINER_SEE_H

#include "sprite.h"
#include "task.h"

bool8 CheckForTrainersWantingBattle(void);
void SetBuriedTrainerMovement(struct ObjectEvent *objEvent);
void DoTrainerApproach(void);

#endif // GUARD_TRAINER_SEE_H
