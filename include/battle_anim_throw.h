#ifndef GUARD_BATTLE_ANIM_THROW_H
#define GUARD_BATTLE_ANIM_THROW_H

void TryShinyAnimation(u8, struct Pokemon *);
u8 ItemIdToBallId(u16);
u8 AnimateBallOpenParticles(u8, u8, u8, u8, u8);
u8 LaunchBallFadeMonTask(u8, u8, u32, u8);

#endif // GUARD_BATTLE_ANIM_THROW_H
