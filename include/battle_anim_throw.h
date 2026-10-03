#ifndef GUARD_BATTLE_ANIM_THROW_H
#define GUARD_BATTLE_ANIM_THROW_H

void TryShinyAnimation(u8 battler, struct Pokemon *mon);
u8 ItemIdToBallId(u16 ballItem);
u8 AnimateBallOpenParticles(u8 x, u8 y, u8 priority, u8 subpriority, u8 ballId);
u8 LaunchBallFadeMonTask(bool8 unfadeLater, u8 spritePalNum, u32 selectedPalettes, u8 ballId);

#endif // GUARD_BATTLE_ANIM_THROW_H
